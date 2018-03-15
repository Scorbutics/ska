cmake_minimum_required(VERSION 3.9)
project(ska_physic_simulation)

set(SDL2_INCLUDE_DIR ${CMAKE_PREFIX_PATH}/include)
set(LIBRARY_DIR ${SDL2_INCLUDE_DIR}/../lib)

message("lib folder is " ${LIBRARY_DIR})

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

link_directories(${LIBRARY_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/G++/${SKA_DEBUG_DIR})

if(MINGW OR CMAKE_COMPILER_IS_GNU)
	message("It's a GNU/MINGW compiler")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lmingw32")
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
else()
	message("It's NOT a GNU/MINGW compiler")
endif()

set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_core)
set(SKA_PHYSIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_physic)
set(SKA_PHYSIC_HOME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_physic_homemade)
set(SKA_DRAW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_draw)
set(SKA_WORLD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_world)
set(SKA_GUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_gui)

include_directories(${SDL2_INCLUDE_DIR})
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

set(YOUR_APP_ALL_DEPS ska_world ska_physic_homemade ska_physic ska_gui ska_draw SDL2main ska_core SDL2_ttf SDL2_image SDL2_gif SDL2)
target_link_libraries(ska_physic_simulation ${YOUR_APP_ALL_DEPS})
