project(skalang_bytecode_generator)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)

link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_BYTECODE_GEN_DIR="${SKALANG_BYTECODE_GEN_DIR}")
generate_source_groups("${SOURCES}" "${SKALANG_BYTECODE_GEN_DIR}\\src\\Generator")
add_library(skalang_bytecode_generator ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(skalang_bytecode_generator skalang skalog skabase Threads::Threads)
target_include_directories(skalang_bytecode_generator PUBLIC ${SKA_BASE_DIR} PUBLIC ${SKA_LOG_DIR} PUBLIC ${SKALANG_BYTECODE_GEN_DIR}/src)
