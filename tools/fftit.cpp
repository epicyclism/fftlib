//
// Copyright (c) 2008-2022 Paul Ranson, paul@epicyclism.com
//
// Refer to licence in repository.
//

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <functional>

#include "fftlib.h"
#include "mm_file.h"

void Welcome()
{
	std::cerr << "FFTit 2.00 Copyright Paul Ranson (c) 2009-2022\n";
	std::cerr << "email - paul@epicyclism.com\n\n";
	std::cerr << "Performs FFT on a file of raw sample data\n";
	std::cerr << "(sizeof fp type is " << sizeof(fp_t) << ")\n\n";
}

void Usage()
{
	std::cerr << "Performs FFTs on a file of raw sample data\n";
	std::cerr << "Usage : FFTit [-Fn] [-D] [-1] [-Wn] <input file> [sample rate]\n";
	std::cerr << "Where input file is a packed array of floats. Output is text to stdout.\n";
	std::cerr << "Options. -Fn, use an FFT width of 2^n.\n";
	std::cerr << "              n between 8 for 256 and 24 for 16777216.\n";
	std::cerr << "              Default is 18 for 262144\n";
	std::cerr << "         -D,  output in dB scaled so 1.0 is 0dB\n";
	std::cerr << "         -1,  perform a single FFT on the centre FFT width samples of the\n";
	std::cerr << "              input, otherwise (default) process the entire file and average\n";
	std::cerr << "              the results of each FFT.\n";
	std::cerr << "         -Wn, select a window function. 0 is no window.";
	std::cerr << "				1 is Hamming and the default.\n";
	std::cerr << "              2 is Blackman, 3 Blackman-Harris.\n";
	std::cerr << "              4 is Kaiser5,  5 Kaiser7.\n";
	std::cerr << "And if you provide the sample rate, the centre frequencies of each bin are written to the output.\n\n";
}

int main(int argc, char* argv[])
{
	Welcome();

	if (argc < 2)
	{
		Usage();

		return -1;
	}
	int		nInFileArg = 0;
	size_t  fftWidth = 18;
	bool    bDB = false;
	bool    bOnce = false;
	size_t sample_rate = -1;
	window_t wt = window_t::HAMMING;

	int		arg = 1;
	while (arg < argc)
	{
		if (argv[arg][0] == '-' || argv[arg][0] == '/')
		{
			switch (argv[arg][1])
			{
			case 'F':
			case 'f':
				fftWidth = atoi(argv[arg] + 2);
				break;
			case 'D':
			case 'd':
				bDB = true;
				break;
			case '1':
				bOnce = true;
				break;
			case 'W':
			case 'w':
				wt = wt_from_code(argv[arg][2]);
				break;
			default:
				std::cerr << "Unknown argument \'" << argv[arg][1] << "\'!\n";
				Usage();
				return -1;
			}
		}
		else
		{
			if (nInFileArg == 0)
			{
				nInFileArg = arg;
			}
			else
			{
				sample_rate = ::atoi(argv[arg]);
			}
		}
		++arg;
	}
	// checks
	if (sample_rate == 0)
	{
		std::cerr << "Sample rate provided was not understood\n";
		Usage();
		return -1;
	}
	if (fftWidth < FFTWdMin || fftWidth > FFTWdMax)
	{
		std::cerr << "FFTWidth provided is out of range, valid between" << FFTWdMin << " and " << FFTWdMax << " inclusive.\n";
		Usage();
		return -1;
	}
	mem_map_file<fp_t> mmf(argv[nInFileArg]);
	if (!mmf)
	{
		std::cerr << "Couldn't open <" << argv[nInFileArg] << ">\n";

		return -1;
	}

	// an FFT implementation!
	auto pfft = make_fft(fftWidth, wt);
	std::vector<fp_t> mean(pfft->width());

	// report
	std::cerr << "FFTit. Processing,  width " << pfft->width() << ", window " << wt_to_string(wt) << "\n";

	if (bOnce)
	{
		if (mmf.length() < pfft->width())
		{
			std::cerr << "Insufficient signal supplied for the specified FFT width\n";

			return -1;
		}
		size_t offset = (mmf.length() - pfft->width()) / 2;
		// just a single effort
		auto[ob, oe] = (*pfft) (mmf.ptr() + offset, mmf.ptr() + offset + pfft->width());
		std::copy(ob, oe, mean.begin());
	}
	else
	{
		// 50% overlap
		size_t nffts = mmf.length();
		if (nffts >= pfft->width() * 3 / 2)
		{
			nffts /= (pfft->width() / 2);
			nffts -= 1;
		}
		else
		if (nffts > pfft->width())
			nffts = 1;
		else
		{
			std::cerr << "Insufficient signal supplied for the specified FFT width\n";

			return -1;
		}

		for (size_t n = 0; n < nffts; ++n)
		{
			auto[ob, oe] = (*pfft) (mmf.ptr() + n * pfft->width() / 2, mmf.ptr() + n * pfft->width() / 2 + pfft->width());
			// add to average
			std::transform(mean.begin(), mean.end(), ob, mean.begin(), std::plus<>());
		}
		using namespace std::placeholders;
		std::transform(mean.begin(), mean.end(), mean.begin(), std::bind(std::divides<fp_t>(), _1, fp_t(nffts)));
	}
	if (sample_rate != -1)
	{
		double fb = 0;
		double fbinc = double(sample_rate) / pfft->width();
		for (size_t i = 0; i < pfft->width() / 2; ++i)
		{
			fp_t out;
			if (bDB)
				out = fp_t(20.0) * log10(mean[i]);
			else
				out = mean[i];

			std::cout << fb << " " << out << "\n";
			fb += fbinc;
		}
	}
	else
	{
		for (size_t i = 0; i < pfft->width() / 2; ++i)
		{
			fp_t out;
			if (bDB)
				out = fp_t(20.0) * log10(mean[i]);
			else
				out = mean[i];
			std::cout << out << "\n";
		}
	}

	return 0;
}

