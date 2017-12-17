cmake_minimum_required(VERSION 3.9)
project(ska_core)

find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
find_library(SDL2_LIBRARY NAME SDL2)

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

add_library(ska_core STATIC ${SOURCES})
set_target_properties(ska_core PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../${SKA_DEBUG_DIR})
include_directories(${SDL2_INCLUDE_DIR})