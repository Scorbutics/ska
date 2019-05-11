project(ska_ai)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${SKA_PHYSIC_CHIPMUNK_DIR})
include_directories(${GSL_DIR})
include_directories(${CHIPMUNK_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(ska_ai ${SOURCES})
