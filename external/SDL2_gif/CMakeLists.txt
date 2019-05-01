project(SDL2_gif)

link_directories(${LIB_DIR})
link_directories(${EXTERNAL_LIB_DIR})

include_directories(${SDL2_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.h
    src/*.c
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(SDL2_gif ${SOURCES})
target_link_libraries(SDL2_gif ska_core SDL2)
