cmake_minimum_required(VERSION 3.9)
project(skabase)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(SKA_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/src)
set(SKALOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/skalog/src/src)
set(GSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/gsl)
set(DOCTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/doctest)
set(FAKEIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/fakeit)

add_subdirectory(external/skalog)
add_subdirectory(src)
add_subdirectory(test)
