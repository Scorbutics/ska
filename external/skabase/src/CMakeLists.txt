project(skabase)

include_directories(${SKALOG_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${GSL_DIR})

link_directories(../lib)

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(skabase ${SOURCES})
target_link_libraries(skabase skalog)
target_include_directories(skabase PUBLIC ../external/skalog/src/src)
