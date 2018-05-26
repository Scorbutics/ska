cmake_minimum_required(VERSION 3.9)
project(ska_ai_test)

set(CMAKE_CXX_STANDARD 17)

set(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/SDL2)

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/${CMAKE_CXX_COMPILER_ID}/${SKA_DEBUG_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})

set(DOCTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest)
set(FAKEIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/fakeit)
set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_core)
set(SKA_AI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_ai)
set(GSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/gsl)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${DOCTEST_DIR})
include_directories(${FAKEIT_DIR})
include_directories(${GSL_DIR})
include_directories(${SKA_AI_DIR})
include_directories(${SKA_CORE_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_ai_test ${SOURCES})
set_target_properties(ska_ai_test PROPERTIES
					  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_RELEASE_DIR})

target_link_libraries(ska_ai_test ska_ai ska_core)
