project(skalang_test)

include_directories(${SKALANG_SRC_DIR}/src)

link_directories(../lib)

#Unit Tests part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_TEST_DIR="${SKALANG_TEST_DIR}")
add_definitions(-DSKALANG_SRC_DIR="${SKALANG_SRC_DIR}")
generate_source_groups("${SOURCES}" "${SKALANG_TEST_DIR}\\src")
add_executable(skalang_test ${SOURCES})

find_package(Threads REQUIRED)
set(LIBS skalang skalog skabase Threads::Threads)
IF(UNIX AND NOT EMSCRIPTEN)
	set(LIBS ${LIBS} atomic)
ENDIF()
target_link_libraries(skalang_test ${LIBS})
target_include_directories(skalang_test PUBLIC ../external/skalog/src/src)
target_include_directories(skalang_test PUBLIC ../external/skabase/src/src)
target_include_directories(skalang_test PUBLIC ../external/doctest)

add_subdirectory(${SKALANG_BYTECODE_TEST_DIR})
