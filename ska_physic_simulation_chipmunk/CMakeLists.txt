cmake_minimum_required(VERSION 3.9)
project(ska_physic_simulation_chipmunk)

set(CMAKE_CXX_STANDARD 17)

set(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/SDL2)

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/${CMAKE_CXX_COMPILER_ID}/${SKA_DEBUG_DIR})

set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_core)
set(SKA_PHYSIC_CHIPMUNK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_physic_chipmunk)
set(SKA_DRAW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_draw)
set(SKA_WORLD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_world)
set(SKA_GUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_gui)
set(CHIPMUNK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/chipmunk/include/chipmunk)
set(SDL2_GIF_INCLUDE_DIR ${CMAKE_BINARY_DIR}/../external/SDL2_gif/src)
set(GSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/gsl)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_GIF_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SKA_PHYSIC_CHIPMUNK_DIR})
include_directories(${SKA_WORLD_DIR})
include_directories(${SKA_GUI_DIR})
include_directories(${CHIPMUNK_DIR})
include_directories(${GSL_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_physic_simulation_chipmunk ${SOURCES})
set_target_properties(ska_physic_simulation_chipmunk PROPERTIES
					  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_RELEASE_DIR})

set(YOUR_APP_ALL_DEPS ska_world ska_physic_chipmunk ska_physic_homemade ska_gui ska_draw ska_core chipmunk)
target_link_libraries(ska_physic_simulation_chipmunk ${YOUR_APP_ALL_DEPS})
