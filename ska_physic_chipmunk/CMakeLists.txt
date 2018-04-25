cmake_minimum_required(VERSION 3.9)
project(ska_physic_chipmunk)

find_path(SDL2_INCLUDE_DIR NAME SDL.h)
if(NOT SDL2_INCLUDE_DIR)
	set(SDL2_INCLUDE_DIR ${CMAKE_PREFIX_PATH}/include)
endif()
set(SDL2_GIF_INCLUDE_DIR ${CMAKE_BINARY_DIR}/../external/SDL2_gif/src)
set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_core)
set(SKA_PHYSIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_physic)
set(CHIPMUNK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/chipmunk)
set(GSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/gsl)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_GIF_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_PHYSIC_DIR})
include_directories(${CHIPMUNK_DIR})
include_directories(${GSL_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

add_library(ska_physic_chipmunk STATIC ${SOURCES})
set_target_properties(ska_physic_chipmunk PROPERTIES
					  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_RELEASE_DIR})