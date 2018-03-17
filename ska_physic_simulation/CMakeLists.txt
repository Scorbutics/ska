cmake_minimum_required(VERSION 3.9)
project(ska_physic_simulation)

find_path(SDL2_INCLUDE_DIR NAME SDL.h)
if(NOT SDL2_INCLUDE_DIR)
	set(SDL2_INCLUDE_DIR ${CMAKE_PREFIX_PATH}/include)
endif()
set(LIBRARY_DIR ${SDL2_INCLUDE_DIR}/../lib)

message("lib folder is " ${LIBRARY_DIR})

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(SKA_COMPILER_NAME "CLANG")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(SKA_COMPILER_NAME "G++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(SKA_COMPILER_NAME "MSVC")
endif()

link_directories(${LIBRARY_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/${SKA_COMPILER_NAME}/${SKA_DEBUG_DIR})

set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_core)
set(SKA_PHYSIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_physic)
set(SKA_PHYSIC_HOME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_physic_homemade)
set(SKA_DRAW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_draw)
set(SKA_WORLD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_world)
set(SKA_GUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_gui)
set(SDL2_GIF_INCLUDE_DIR ${CMAKE_BINARY_DIR}/../external/SDL2_gif/src)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_GIF_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SKA_PHYSIC_DIR})
include_directories(${SKA_PHYSIC_HOME_DIR})
include_directories(${SKA_WORLD_DIR})
include_directories(${SKA_GUI_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_physic_simulation ${SOURCES})
set_target_properties(ska_physic_simulation PROPERTIES
					  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_RELEASE_DIR})

set(YOUR_APP_ALL_DEPS ska_world ska_physic_homemade ska_physic ska_gui ska_draw ska_core SDL2_ttf SDL2_image SDL2_gif SDL2)
target_link_libraries(ska_physic_simulation ${YOUR_APP_ALL_DEPS})
