cmake_minimum_required(VERSION 3.9)
project(ska_particle_benchmarker)

set(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/SDL2)

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_core)
set(SKA_DRAW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_draw)
set(SKA_PARTICLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_particle)
set(GSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/gsl)
set(SDL2_GIF_INCLUDE_DIR ${CMAKE_BINARY_DIR}/../external/SDL2_gif/src)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_PARTICLE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${GSL_DIR})
include_directories(${SDL2_GIF_INCLUDE_DIR})

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/${CMAKE_CXX_COMPILER_ID}/${SKA_DEBUG_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_particle_benchmarker ${SOURCES})
set_target_properties(ska_particle_benchmarker PROPERTIES
					  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_RELEASE_DIR})

set(YOUR_APP_ALL_DEPS ska_particle ska_draw ska_core SDL2_gif SDL2main SDL2_ttf SDL2_image SDL2)
target_link_libraries(ska_particle_benchmarker ${YOUR_APP_ALL_DEPS})
