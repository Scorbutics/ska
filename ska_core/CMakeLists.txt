cmake_minimum_required(VERSION 3.9)
project(ska_core)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(skalog)

set(SDL2_INCLUDE_DIR ../external/SDL2)
set(GSL_DIR ../external/gsl)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${GSL_DIR})
include_directories(skalog/src/src)

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(ska_core STATIC ${SOURCES})

