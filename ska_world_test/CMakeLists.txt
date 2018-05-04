cmake_minimum_required(VERSION 3.9)
project(ska_world_test)

find_path(SDL2_INCLUDE_DIR NAME SDL.h)
if(NOT SDL2_INCLUDE_DIR)
	set(SDL2_INCLUDE_DIR ${CMAKE_PREFIX_PATH}/include)
endif()

set(LIBRARY_DIR ${SDL2_INCLUDE_DIR}/../lib)

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

set(DOCTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest)
set(FAKEIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/fakeit)
set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_core)
set(SKA_DRAW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_draw)
set(SKA_WORLD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_world)
set(GSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/gsl)
set(SDL2_GIF_INCLUDE_DIR ${CMAKE_BINARY_DIR}/../external/SDL2_gif/src)

include_directories(${GSL_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_GIF_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${DOCTEST_DIR})
include_directories(${FAKEIT_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SKA_WORLD_DIR})

link_directories(${LIBRARY_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")
set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

add_executable(ska_world_test ${SOURCES})
set_target_properties(ska_world_test PROPERTIES
					  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_RELEASE_DIR})

target_link_libraries(ska_world_test ska_world ska_draw ska_core SDL2_ttf SDL2_image SDL2_gif SDL2)
