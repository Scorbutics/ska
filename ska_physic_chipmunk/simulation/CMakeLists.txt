project(ska_physic_simulation_chipmunk)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_GIF_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SKA_PHYSIC_CHIPMUNK_DIR})
include_directories(${SKA_WORLD_DIR})
include_directories(${SKA_GUI_DIR})
include_directories(${CHIPMUNK_DIR})
include_directories(${GSL_DIR})

link_directories(${LIB_DIR})
link_directories(${EXTERNAL_LIB_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_physic_simulation_chipmunk ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${SKA_DIR}")
set(ALL_DEPS ska_world ska_physic_chipmunk ska_physic_homemade ska_gui ska_draw ska_core skalog chipmunk)
target_link_libraries(ska_physic_simulation_chipmunk ${ALL_DEPS})
