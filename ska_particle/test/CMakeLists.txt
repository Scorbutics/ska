project(ska_particle_test)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${DOCTEST_DIR})
include_directories(${FAKEIT_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SKA_GUI_DIR})
include_directories(${GSL_DIR})
include_directories(${SKA_PARTICLE_DIR})

link_directories(${EXTERNAL_LIB_DIR})
link_directories(${LIB_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_particle_test ${SOURCES})

find_package(Threads REQUIRED)
set(ALL_DEPS ska_gui ska_draw ska_core ska_particle skalog Threads::Threads)
target_link_libraries(ska_particle_test ${ALL_DEPS})
