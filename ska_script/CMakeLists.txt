cmake_minimum_required(VERSION 3.9)
project(ska_script)

set(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/SDL2)
set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_core)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

add_library(ska_script STATIC ${SOURCES})
set_target_properties(ska_script PROPERTIES
					  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_RELEASE_DIR})