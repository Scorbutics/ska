project(ska_physic_simulation)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_GIF_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SKA_PHYSIC_HOME_DIR})
include_directories(${SKA_WORLD_DIR})
include_directories(${SKA_GUI_DIR})
include_directories(${GSL_DIR})

link_directories(${LIB_DIR})
link_directories(${EXTERNAL_LIB_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_physic_simulation ${SOURCES})

set(ALL_DEPS ska_world ska_physic_homemade ska_gui ska_draw ska_core skalog)
target_link_libraries(ska_physic_simulation ${ALL_DEPS})
