# fftlib

A basic library for doing FFTs. Neither super optimised or particularly flexible. Good for repeated transformations. Nowadays expects C++20.

There is an example usage in the 'tools' director, fftit. Accompanied by a simple signal generating program.

fftit takes input from a binary file of packed floating point values and writes the spectrum to stdout in a way that is straigtforward to render with, for example, gnuplot.
fm_generate can produce a 'pure' tone, or frequency modulate that tone to make things more interesting.

```
// produce a minute of 1kHz tone, based on a sample rate of 16000Hz
fm_generate.exe 1000 0 0 16000 60 .\1kHz.raw
// transform, output in decibels, default Hamming window, 65536 point FFT, 
// repeat FFT along input and average resuls
fftit -F16 -D .\1kHz.raw 16000 > .\1khz_spec.dat
```

Or for something a bit prettier,

```
fm_generate.exe 1000 100 0.2 16000 60 .\1kHz_fm.raw
fftit -F16 -D .\1kHz_fm.raw 16000 > .\1khz_fm_spec.dat
```

No warranty, bound to be buggy. This code originates before testing was a thing and has been partially updated to more modern standards.