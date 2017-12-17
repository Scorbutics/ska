cmake_minimum_required(VERSION 3.9)
project(ska_draw)

set(SDL2_INCLUDE_DIR "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.11.25503/include")
set(SDL2_GIF_INCLUDE_DIR "../external/SDL2_gif/src")

set(SKA_CORE_DIR "../ska_core")
set(SKA_CORE_HEADER_FILES ${SKA_CORE_DIR}/*.h)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_GIF_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${SDL2_GIF_INCLUDE_DIR}/*.h
	${SKA_CORE_DIR}/*.h
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

add_library(ska_draw STATIC ${SOURCES})
set_target_properties(ska_draw PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../${SKA_RELEASE_DIR})