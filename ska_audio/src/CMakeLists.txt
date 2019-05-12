project(ska_audio)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_AUDIO_DIR})
include_directories(${SKA_BASE_DIR})
include_directories(${GSL_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")


add_library(ska_audio ${SOURCES})
target_link_libraries(ska_audio skabase skalog ska_core)
target_include_directories(ska_audio PUBLIC ../../external/skabase/src/src PUBLIC ../../external/skalog/src/src)