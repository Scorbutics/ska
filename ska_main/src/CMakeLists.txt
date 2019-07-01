project(ska_main)

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(ska_main ${SOURCES})

find_package(Threads REQUIRED)
set(TARGET_LIBS skabase skalog Threads::Threads)
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(TARGET_LIBS ${TARGET_LIBS} dl)
endif()
target_link_libraries(ska_main ${TARGET_LIBS})
target_include_directories(ska_main PUBLIC ../../external/skabase/src/src PUBLIC ../../external/skalog/src/src)
