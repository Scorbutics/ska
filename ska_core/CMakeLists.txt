cmake_minimum_required(VERSION 3.9)
project(ska_core)

find_path(SDL2_INCLUDE_DIR NAME SDL.h)
if(NOT SDL2_INCLUDE_DIR)
	set(SDL2_INCLUDE_DIR ${CMAKE_PREFIX_PATH}/include)
endif()

include_directories(${SDL2_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${SDL2_INCLUDE_DIR}/*.h
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
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})
