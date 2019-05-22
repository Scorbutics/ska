project(ska_core)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${GSL_DIR})

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(ska_core ${SOURCES})
set(TARGET_LIBS skalog skabase)
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(TARGET_LIBS ${TARGET_LIBS} dl)
endif()
target_link_libraries(ska_core ${TARGET_LIBS})
target_include_directories(ska_core PUBLIC ../../external/skabase/src/src PUBLIC ../../external/skalog/src/src)
