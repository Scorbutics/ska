project(ska_script_test)

link_directories(${EXTERNAL_LIB_DIR})
link_directories(${LIB_DIR})

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${DOCTEST_DIR})
include_directories(${FAKEIT_DIR})
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

add_executable(ska_script_test ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${SKA_DIR}")
find_package(Threads REQUIRED)
set(ALL_DEPS ska_world ska_script skalang_std Threads::Threads)
target_link_libraries(ska_script_test ${ALL_DEPS})
