cmake_minimum_required (VERSION 3.18)

project ("fftlib")

set_property(GLOBAL PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${PROJECT_SOURCE_DIR}/fftlib")

add_subdirectory ("fftlib")
add_subdirectory ("tools")
