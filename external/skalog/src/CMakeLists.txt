cmake_minimum_required(VERSION 3.10)
project(skalog)

if(NOT SKALOG_NO_RLUTIL)
include_directories(../external/rlutil)
else()
    message(STATUS "Disabled rlutil support")
    add_compile_definitions(SKALOG_NO_RLUTIL)
endif()

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(skalog ${SOURCES})
set_property(TARGET skalog PROPERTY POSITION_INDEPENDENT_CODE ON)
