project(ska_scripting)

# Scripting with the skalang language : script virtual machine and engine bindings
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_SCRIPTING_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${GSL_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(ska_scripting ${SOURCES})
target_link_libraries(ska_scripting ska_core skalang_std skalang_bytecode_interpreter skalang_bytecode_generator skalang)
target_include_directories(ska_scripting PUBLIC ${SKA_SCRIPTING_DIR})
add_dependencies(ska_scripting skalang_std_scripts)
