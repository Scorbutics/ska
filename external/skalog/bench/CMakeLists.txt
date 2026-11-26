cmake_minimum_required(VERSION 3.10)
project(skalog_bench)

set(CMAKE_CXX_STANDARD 17)

include_directories(../src/src)
include_directories(external/benchmark/include)

link_directories(../lib)
link_directories(external/build/src)
link_directories(external/build/lib)

file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(skalog_bench ${SOURCES})
find_package(Threads REQUIRED)

set(LIBS skalog benchmark)
IF(WIN32)
    set(LIBS ${LIBS} shlwapi)
ENDIF(WIN32)

target_link_libraries(skalog_bench ${LIBS} Threads::Threads)

