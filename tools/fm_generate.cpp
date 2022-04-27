//
// Copyright (c) 2008-2022 Paul Ranson, paul@epicyclism.com
//
// Refer to licence in repository.
//

#include <iostream>
#include <vector>
#include <cstdlib>

#include "basic_file.h"
#include "fftlib.h"

void Usage()
{
	std::cerr << "Generates a raw PCM file containing 'fm'\n";
	std::cerr << "Usage - Fm_generate <carrier frequency> <modulation frequency> <deviation> <sample rate> <duration> <outputfile>\n";
	std::cerr << "Where duration is in seconds. Output is a packed array of F.\n";
	std::cerr << "(If modulation frequency or deviation are 0 then a pure sine is generated.)\n";
	std::cerr << "For example - FMGenerate 3150 4.5 1 96000 30 3150_4_5.raw\n\n";
	std::cerr << "(sizeof fp_t is " << sizeof(fp_t) << ")\n\n";
}

int main(int argc, char* argv[])
{
	if (argc < 7)
	{
		Usage();
		return 1;
	}
	fp_t carrier;
	fp_t modulation;
	fp_t deviation;
	size_t sample_rate;
	size_t duration;

	carrier = (fp_t)::atof(argv[1]);
	modulation = (fp_t)::atof(argv[2]);
	deviation = (fp_t)::atof(argv[3]);
	sample_rate = ::atoi(argv[4]);
	duration = ::atoi(argv[5]);

	// validate
	if (carrier < 1.0 || carrier >(fp_t)sample_rate / 2)
	{
		std::cerr << "Carrier <" << carrier << "> is out of range.\n";
		Usage();
		return 1;
	}

	// open output file
	out_file_t of(argv[6]);
	if (!of)
	{
		std::cerr << "Couldn't open output file <" << argv[6] << ">\n";
		return -1;
	}

	// allocate
	std::vector<fp_t> buf;
	buf.resize(sample_rate * duration);

	// generate
	if (modulation == fp_t(0) || deviation == fp_t(0))
	{
		fill_buffer_with_sine(carrier, buf.data(), buf.data() + buf.size(), sample_rate);
	}
	else
	{
		fill_buffer_with_FM(carrier, modulation, deviation, buf.data(), buf.data() + buf.size(), sample_rate);
	}

	// write
	of.write(buf.data(), buf.size() * sizeof(fp_t));

	return 0;
}

