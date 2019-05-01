project(ska_core)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${GSL_DIR})
include_directories(${SKALOG_DIR})

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(ska_core ${SOURCES})

