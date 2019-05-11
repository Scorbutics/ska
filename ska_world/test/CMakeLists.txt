project(ska_world_test)

include_directories(${GSL_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_GIF_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${DOCTEST_DIR})
include_directories(${FAKEIT_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SKA_WORLD_DIR})

link_directories(${LIB_DIR})
link_directories(${EXTERNAL_LIB_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_world_test ${SOURCES})
target_link_libraries(ska_world_test ska_world ska_draw ska_core skalog)
