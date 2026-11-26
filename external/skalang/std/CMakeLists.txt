project(skalang_std)

include_directories(${SKA_BASE_DIR})
include_directories(src)
include_directories(${SKALANG_SRC_DIR}/src)
include_directories(${SKALANG_INTERPRETER_DIR}/src)
include_directories(${SKALANG_BYTECODE_GEN_DIR}/src)
include_directories(${SKALANG_BYTECODE_INT_DIR}/src)

# The native modules are header-only : this target gives their include directory to the users of the standard library
add_library(skalang_std INTERFACE)
target_include_directories(skalang_std INTERFACE ${SKALANG_STD_DIR}/src)
target_link_libraries(skalang_std INTERFACE skalang_bytecode_interpreter)

# Directory of the standard library scripts (to be deployed with a program using skalang)
set(SKALANG_STD_SCRIPTS_DIR ${SKALANG_STD_DIR}/src/std/scripts CACHE INTERNAL "skalang standard library scripts directory")
