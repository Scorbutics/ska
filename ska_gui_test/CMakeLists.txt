cmake_minimum_required(VERSION 3.9)
project(ska_gui_test)

find_path(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
if(NOT SDL2_INCLUDE_DIR)
	set(SDL2_INCLUDE_DIR ${CMAKE_PREFIX_PATH}/include)
endif()

set(LIBRARY_DIR ${SDL2_INCLUDE_DIR}/../lib)

set(DOCTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest)
set(FAKEIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/fakeit)
set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_core)
set(SKA_DRAW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_draw)
set(SKA_GUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ska_gui)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${DOCTEST_DIR})
include_directories(${FAKEIT_DIR})
include_directories(${SKA_CORE_DIR})
include_directories(${SKA_DRAW_DIR})
include_directories(${SKA_GUI_DIR})

file(GLOB_RECURSE SOURCES
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${SKA_CORE_DIR}/*.h
	${SKA_DRAW_DIR}/*.h
	${SKA_GUI_DIR}/*.h
    *.h
    *.cpp
)

string(REGEX REPLACE "CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

set(SKA_DEBUG_DIR "Debug")
set(SKA_RELEASE_DIR "Release")

add_executable(ska_gui_test ${SOURCES})
set_target_properties(ska_gui_test PROPERTIES
					  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_RELEASE_DIR})

find_library(SKA_CORE_LIB NAMES libska_core PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})
if(NOT SKA_CORE_LIB)
  message(FATAL_ERROR "ska core library not found")
endif()

find_library(SKA_DRAW_LIB NAMES libska_draw PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})
if(NOT SKA_DRAW_LIB)
  message(FATAL_ERROR "ska draw library not found")
endif()

find_library(SKA_GUI_LIB NAMES libska_gui PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../${SKA_DEBUG_DIR})
if(NOT SKA_GUI_LIB)
  message(FATAL_ERROR "ska gui library not found")
endif()

find_library(SDL2_LIB NAMES SDL2 libSDL2 PATHS ${LIBRARY_DIR})
if(NOT SDL2_LIB)
  message(FATAL_ERROR "SDL2 library not found")
endif()

find_library(SDL2_TTF_LIB NAMES SDL2_ttf libSDL2_ttf PATHS ${LIBRARY_DIR})
if(NOT SDL2_TTF_LIB)
  message(FATAL_ERROR "SDL2 ttf library not found")
endif()

find_library(SDL2_IMG_LIB NAMES SDL2_image libSDL_image PATHS ${LIBRARY_DIR})
if(NOT SDL2_IMG_LIB)
  message(FATAL_ERROR "SDL2 image library not found")
endif()


target_link_libraries(ska_gui_test ${SDL2_LIB})
target_link_libraries(ska_gui_test ${SDL2_TTF_LIB})
target_link_libraries(ska_gui_test ${SDL2_IMG_LIB})
target_link_libraries(ska_gui_test ${SKA_CORE_LIB})
target_link_libraries(ska_gui_test ${SKA_DRAW_LIB})
target_link_libraries(ska_gui_test ${SKA_GUI_LIB})
