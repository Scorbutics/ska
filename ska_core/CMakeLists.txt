cmake_minimum_required(VERSION 3.9)
project(ska_core)

set(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/SDL2)
set(GSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/gsl)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${GSL_DIR})

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
					  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_RELEASE_DIR})
