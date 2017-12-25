cmake_minimum_required(VERSION 3.9)
project(ska_core_test)

find_path(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
if(NOT SDL2_INCLUDE_DIR)
	set(SDL2_INCLUDE_DIR ${CMAKE_PREFIX_PATH}/include)
endif()

set(LIBRARY_DIR ${SDL2_INCLUDE_DIR}/../lib)

message("lib folder is " ${LIBRARY_DIR})

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

link_directories(${LIBRARY_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})

if(MINGW OR CMAKE_COMPILER_IS_GNU)
	message("It's a GNU/MINGW compiler")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lmingw32")
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
else()
	message("It's NOT a GNU/MINGW compiler")
endif()

set(DOCTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest)
set(FAKEIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/fakeit)
set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_core)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${DOCTEST_DIR})
include_directories(${FAKEIT_DIR})
include_directories(${SKA_CORE_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_core_test ${SOURCES})
set_target_properties(ska_core_test PROPERTIES
					  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_RELEASE_DIR})

target_link_libraries(ska_core_test SDL2main ska_core SDL2)
