project(skalang)

include_directories(src)

link_directories(../lib)

file(GLOB_RECURSE SOURCES
    LIST_DIRECTORIES false
	src/*.h
	src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_definitions(-DSKALANG_SRC_DIR="${SKALANG_SRC_DIR}")

function(generate_source_groups src_files src_dir)
    foreach(_source IN ITEMS ${src_files})
        get_filename_component(_source_path "${_source}" PATH)
        string(REPLACE "/" "\\" _group_path "${_source_path}")
        string(REPLACE "/" "\\" src_dir "${src_dir}")
        string(REPLACE "${src_dir}" "" _group_path "${_group_path}")
        source_group("${_group_path}" FILES "${_source}")
    endforeach()
endfunction()

generate_source_groups("${SOURCES}" "${SKALANG_SRC_DIR}\\src")

add_library(skalang ${SOURCES})
find_package(Threads REQUIRED)
target_link_libraries(skalang skalog skabase Threads::Threads)
target_include_directories(skalang PUBLIC ${SKA_BASE_DIR} PUBLIC ${SKA_LOG_DIR} PUBLIC ${SKALANG_SRC_DIR}/src)
