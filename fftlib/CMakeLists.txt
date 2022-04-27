cmake_minimum_required (VERSION 3.18)

# Add source to this project's executable.
add_library (fftlib fftlib.cpp fftlib.h FFT.h FFTImpl.h ProcFFT.h ProcFFTImpl.h)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET fftlib PROPERTY CXX_STANDARD 20)
endif()
