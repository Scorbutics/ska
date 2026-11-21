project(ska_scripting_test)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${DOCTEST_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_SCRIPTING_DIR})
include_directories(${SKA_BASE_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKA_SCRIPTING_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(ska_scripting_test ${SOURCES})
find_package(Threads REQUIRED)
target_link_libraries(ska_scripting_test ska_scripting Threads::Threads)
