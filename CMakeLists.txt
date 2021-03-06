cmake_minimum_required(VERSION 3.9)
project(ska)

set(BUILD_SHARED_LIBS TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
  MESSAGE("++ 64 bit architecture")
  set(SKA_ARCHITECTURE "x64")
 else()
  MESSAGE("++ 32 bit architecture")
  set(SKA_ARCHITECTURE "x86")
endif()

if("${CMAKE_BUILD_TYPE}" EQUAL "")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

MESSAGE("++ " ${CMAKE_BUILD_TYPE} " build type")

#Internal
set(SKA_DIR ${CMAKE_BINARY_DIR})
set(SKA_AI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_ai/src/src)
set(SKA_AUDIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_audio/src/src)
set(SKA_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_core/src/src)
set(SKA_DRAW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_draw/src/src)
set(SKA_GUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_gui/src/src)
set(SKA_PHYSIC_CHIPMUNK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_physic_chipmunk/src/src)
set(SKA_PHYSIC_HOME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_physic_homemade/src/src)
set(SKA_WORLD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_world/src/src)
set(SKA_PARTICLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_particle/src/src)
set(SKA_SCRIPT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_script/src/src)

#External
set(SKA_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/skabase/src/src)
set(SKALOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/skalog/src/src)
set(GSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/gsl)
set(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2)
set(SDL2GIF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_gif/src)
set(DOCTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/doctest)
set(CHIPMUNK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/chipmunk/include)
set(FAKEIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/fakeit)
set(SDL2_GIF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_gif/src)

#Linking
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(EXTERNAL_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external_lib/${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE}/${SKA_ARCHITECTURE})

include_directories(${SKALOG_DIR})

add_subdirectory(external/skalog/src)
add_subdirectory(external/skabase/src)
#add_subdirectory(external/SDL2_gif)
set(BUILD_DEMOS OFF CACHE BOOL "No demo for chipmunk" FORCE )
add_subdirectory(external/chipmunk)

add_subdirectory(ska_ai)
add_subdirectory(ska_audio)
add_subdirectory(ska_main)
add_subdirectory(ska_core)
add_subdirectory(ska_draw)
add_subdirectory(ska_gui)
add_subdirectory(ska_particle)
add_subdirectory(ska_world)
add_subdirectory(ska_physic_chipmunk)
add_subdirectory(ska_physic_homemade)
add_subdirectory(ska_script)


