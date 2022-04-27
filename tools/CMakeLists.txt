cmake_minimum_required (VERSION 3.18)

# Add source to this project's executable.
add_executable (fftit fftit.cpp mm_file.h )
add_executable (fm_generate fm_generate.cpp basic_file.h)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET fftlib PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(fftit fftlib)
target_link_libraries(fm_generate fftlib)