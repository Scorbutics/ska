project(ska_draw)

include_directories(${SKA_CORE_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2GIF_INCLUDE_DIR})
include_directories(${GSL_DIR})

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(ska_draw ${SOURCES})
target_link_libraries(ska_draw skabase skalog ska_core)
target_include_directories(ska_draw PUBLIC ../../external/skabase/src/src PUBLIC ../../external/skalog/src/src)