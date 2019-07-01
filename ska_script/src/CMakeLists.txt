project(ska_script)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKALANG_DIR})
include_directories(${SKALANG_STD_LIB_DIR})
include_directories(${SKA_WORLD_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_SCRIPT_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${GSL_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(ska_script ${SOURCES})
target_link_libraries(ska_script ska_world skalang)
