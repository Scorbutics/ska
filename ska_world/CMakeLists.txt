cmake_minimum_required(VERSION 3.10)
project(ska_world)

set(SDL2_INCLUDE_DIR "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.11.25503/include")

set(SKA_CORE_DIR "../ska_core")
set(SKA_DRAW_DIR "../ska_draw")
set(SKA_PHYSIC_DIR "../ska_physic")

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SKA_PHYSIC_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${SKA_CORE_DIR}/*.h
	${SKA_DRAW_DIR}/*.h
	${SKA_PHYSIC_DIR}/*.h
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")
set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

add_library(ska_world STATIC ${SOURCES})
set_target_properties(ska_world PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../${SKA_DEBUG_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../${SKA_RELEASE_DIR})