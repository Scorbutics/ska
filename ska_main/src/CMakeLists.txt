project(ska_main)

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_main ${SOURCES})
target_link_libraries(ska_main skabase skalog)
target_include_directories(ska_main PUBLIC ../../external/skabase/src/src PUBLIC ../../external/skalog/src/src)
