cmake_minimum_required(VERSION 3.10)
project(doctest)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	*.h
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")
