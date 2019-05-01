project(ska_ai)

find_path(SDL2_INCLUDE_DIR NAME SDL.h)
if(NOT SDL2_INCLUDE_DIR)
	set(SDL2_INCLUDE_DIR ${CMAKE_PREFIX_PATH}/include)
endif()

set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ska_core)
set(SKA_PHYSIC_CHIPMUNK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ska_physic_chipmunk)
set(GSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/gsl)
set(CHIPMUNK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/chipmunk/include/chipmunk)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
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
