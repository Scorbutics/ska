project(skalang_bytecode_serializer)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_BYTECODE_GEN_DIR}/src)
include_directories(${SKALANG_STD_DIR}/src)

link_directories(../lib)

#Runner part
file(GLOB_RECURSE SOURCES
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_BYTECODE_SER_DIR="${SKALANG_BYTECODE_SER_DIR}")
generate_source_groups("${SOURCES}" "${SKALANG_BYTECODE_SER_DIR}\\src")
add_library(skalang_bytecode_serializer ${SOURCES})

find_package(Threads REQUIRED)
set(LIBS skalang_bytecode_generator skalang skalog skabase Threads::Threads)
IF(UNIX)
	set(LIBS atomic ${LIBS})
ENDIF(UNIX)
target_link_libraries(skalang_bytecode_serializer ${LIBS})
target_include_directories(skalang_bytecode_serializer PUBLIC ${SKA_BASE_DIR} PUBLIC ${SKA_LOG_DIR} PUBLIC ${SKALANG_BYTECODE_SER_DIR}/src)
