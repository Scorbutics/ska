project(ska_particle_benchmarker)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_PARTICLE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${GSL_DIR})
include_directories(${SDL2_GIF_INCLUDE_DIR})

link_directories(${LIB_DIR})
link_directories(${EXTERNAL_LIB_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_particle_benchmarker ${SOURCES})

set(ALL_DEPS ska_particle ska_draw ska_core skalog)
target_link_libraries(ska_particle_benchmarker ${ALL_DEPS})
