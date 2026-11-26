project(skalang_check)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_BYTECODE_GEN_DIR}/src)
include_directories(${SKALANG_BYTECODE_SER_DIR}/src)
include_directories(${SKALANG_BYTECODE_INT_DIR}/src)
include_directories(${SKALANG_STD_DIR}/src)

link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_BYTECODE_CHECK_DIR="${SKALANG_BYTECODE_CHECK_DIR}")
generate_source_groups("${SOURCES}" "${SKALANG_BYTECODE_CHECK_DIR}\\src")
add_executable(skalang_check ${SOURCES})

find_package(Threads REQUIRED)
set(LIBS skalang_bytecode_interpreter skalang_bytecode_generator skalang_bytecode_serializer skalang skalog skabase Threads::Threads)
IF(UNIX AND NOT EMSCRIPTEN)
	set(LIBS ${LIBS} atomic)
ENDIF()
target_link_libraries(skalang_check ${LIBS})
target_include_directories(skalang_check PUBLIC ${SKA_BASE_DIR} PUBLIC ${SKA_LOG_DIR})
