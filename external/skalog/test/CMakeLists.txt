cmake_minimum_required(VERSION 3.10)
project(skalog_test)

include_directories(../external/rlutil)
include_directories(../external/doctest)
include_directories(../src/src)
link_directories(../lib)

file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(skalog_test ${SOURCES})
find_package(Threads REQUIRED)
target_link_libraries(skalog_test skalog Threads::Threads)
