cmake_minimum_required(VERSION 3.10)
project(ska_core_test)

set(SDL2_INCLUDE_DIR "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.11.25503/include")

set(SKA_CORE_DIR "../ska_core")
set(SKA_CORE_HEADER_FILES ${SKA_CORE_DIR}/*.h)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${SKA_CORE_DIR}/*.h
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

add_executable(ska_core_test ${SOURCES})
set_target_properties(ska_core_test PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../${SKA_RELEASE_DIR})

find_library(SKA_CORE_LIB ska_core PATHS ../${SKA_DEBUG_DIR})
if(NOT SKA_CORE_LIB)
  message(FATAL_ERROR "ska core library not found")
endif()

target_link_libraries(ska_core_test ${SKA_CORE_LIB})
