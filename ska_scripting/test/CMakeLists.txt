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

# The test scripts compile (except broken.miniska, that tests a compilation failure)
set(SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/scripts)
skalang_check_scripts(ska_scripting_test_scripts_check
	SCRIPTS ${SCRIPTS_DIR}/basic.miniska ${SCRIPTS_DIR}/components.miniska ${SCRIPTS_DIR}/failing.miniska
		${SCRIPTS_DIR}/math.miniska ${SCRIPTS_DIR}/starter.miniska ${SCRIPTS_DIR}/wait.miniska
	BIND_DIRECTORIES ${SKA_SCRIPT_TEMPLATES_DIR})
