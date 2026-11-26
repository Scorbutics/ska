project(skalang_bytecode_interpreter_test)

include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_TEST_DIR}/src)
include_directories(${SKALANG_BYTECODE_GEN_DIR}/src)
include_directories(${SKALANG_BYTECODE_INT_DIR}/src)
include_directories(${SKALANG_STD_DIR}/src)

link_directories(../../../lib)

#Unit Tests part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_BYTECODE_TEST_DIR="${SKALANG_BYTECODE_TEST_DIR}")
add_definitions(-DSKALANG_TEST_DIR="${SKALANG_TEST_DIR}")
generate_source_groups("${SOURCES}" "${SKALANG_BYTECODE_TEST_DIR}\\interpreter\\src")
add_executable(skalang_bytecode_interpreter_test ${SOURCES})

find_package(Threads REQUIRED)
set(LIBS skalang skalang_bytecode_generator skalang_bytecode_interpreter skalog skabase Threads::Threads)
IF(UNIX)
	set(LIBS ${LIBS} atomic)
ENDIF(UNIX)
target_link_libraries(skalang_bytecode_interpreter_test ${LIBS})
target_include_directories(skalang_bytecode_interpreter_test PUBLIC ../../../external/skalog/src/src)
target_include_directories(skalang_bytecode_interpreter_test PUBLIC ../../../external/skabase/src/src)
target_include_directories(skalang_bytecode_interpreter_test PUBLIC ../../../external/doctest)
