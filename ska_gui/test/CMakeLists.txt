project(ska_gui_test)

link_directories(${EXTERNAL_LIB_DIR})
link_directories(${LIB_DIR})

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${DOCTEST_DIR})
include_directories(${FAKEIT_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SKA_GUI_DIR})
include_directories(${GSL_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_gui_test ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${SKA_DIR}")
find_package(Threads REQUIRED)
set(ALL_DEPS ska_gui ska_draw ska_core skalog Threads::Threads)
target_link_libraries(ska_gui_test ${ALL_DEPS})
