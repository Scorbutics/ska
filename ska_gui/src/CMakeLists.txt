project(ska_gui)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_GUI_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${GSL_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(ska_gui ${SOURCES})
target_link_libraries(ska_gui ska_draw ska_core)
target_include_directories(ska_gui PUBLIC ../../external/skabase/src/src PUBLIC ../../external/skalog/src/src)
