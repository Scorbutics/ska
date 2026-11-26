cmake_minimum_required(VERSION 3.9)
project(ska)

# Shared libraries (the game is a library loaded by ska_main) ; on the web, all static, linked in one page (a CMake with
# its own Emscripten platform would build shared ones : the page cannot link them)
if(EMSCRIPTEN)
	set(BUILD_SHARED_LIBS FALSE)
else()
	set(BUILD_SHARED_LIBS TRUE)
endif()
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

# Development console (tools/ska-console : remote eval, screenshots, input, clock) : compiled in the native builds, opened
# only when the environment variable SKA_CONSOLE gives its socket path (tools/ska-run). Not on the web. OFF for a shipped build.
if(EMSCRIPTEN)
	set(SKA_DEV_CONSOLE_DEFAULT OFF)
else()
	set(SKA_DEV_CONSOLE_DEFAULT ON)
endif()
option(SKA_DEV_CONSOLE "Development console (SKA_CONSOLE socket)" ${SKA_DEV_CONSOLE_DEFAULT})
if(SKA_DEV_CONSOLE)
	add_compile_definitions(SKA_DEV_CONSOLE)
endif()

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
set(SKA_SCRIPTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_scripting/src/src)

#External
set(SKA_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/skabase/src/src)
set(SKALOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/skalog/src/src)
set(GSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/gsl)
# SDL3 headers (the libraries are loaded at run time : libSDL3, libSDL3_image, libSDL3_ttf, libSDL3_mixer)
set(SDL3_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3/include)
set(DOCTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/doctest)
set(CHIPMUNK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/chipmunk/include)
set(FAKEIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/fakeit)

#Linking
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(EXTERNAL_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external_lib/${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE}/${SKA_ARCHITECTURE})

include_directories(${SKALOG_DIR})

add_subdirectory(external/skalog/src)
add_subdirectory(external/skabase/src)

# skalang scripting language (uses the skalog and skabase targets above)
add_subdirectory(external/skalang)
# Templates of the engine bindings ("bind:ska.engine") : for the static check of the scripts of a game (skalang_check_scripts)
set(SKA_SCRIPT_TEMPLATES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ska_scripting/src/scripts CACHE INTERNAL "Templates of the ska scripting bindings")
# Its standard library scripts, and the engine bindings ones, are deployed next to the executables, in "std"
add_custom_target(skalang_std_scripts ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${SKALANG_STD_SCRIPTS_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/std
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/ska_scripting/src/scripts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/std
	COMMENT "Deploying the skalang standard library scripts")
# Shaders : ska_compile_shaders() (GLSL to SPIR-V), for the engine tests and the games
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SkaShaders.cmake)

# SDL3_mixer (loaded at run time by ska_audio) is not packaged everywhere : when it is not installed, it is built here
# (with its built-in decoders : mp3, ogg vorbis, wav, flac), next to the engine libraries
find_library(SKA_SDL3_MIXER_LIBRARY NAMES SDL3_mixer)
# The web : the SDL3 libraries are built by tools/web/build_deps.sh, linked in the page (cmake/SkaWeb.cmake)
if(NOT SKA_SDL3_MIXER_LIBRARY AND NOT EMSCRIPTEN)
	message(STATUS "SDL3_mixer is not installed : it is built from its sources")
	include(FetchContent)
	FetchContent_Declare(SDL3_mixer GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git GIT_TAG release-3.2.4 GIT_SHALLOW TRUE)
	foreach(option SDLMIXER_TESTS SDLMIXER_EXAMPLES SDLMIXER_VENDORED SDLMIXER_FLAC_LIBFLAC SDLMIXER_GME SDLMIXER_MOD SDLMIXER_MIDI SDLMIXER_OPUS SDLMIXER_MP3_MPG123 SDLMIXER_VORBIS_VORBISFILE SDLMIXER_WAVPACK SDLMIXER_INSTALL)
		set(${option} OFF CACHE BOOL "" FORCE)
	endforeach()
	FetchContent_MakeAvailable(SDL3_mixer)
endif()

set(BUILD_DEMOS OFF CACHE BOOL "No demo for chipmunk" FORCE )
add_subdirectory(external/chipmunk)
# Chipmunk adds -ffast-math to its Release flags : its sleeping bodies (islands) compare with INFINITY (static bodies,
# idle times), which fast math assumes never happens. cpSpaceStep loops forever in Release (cpBodyActivate)
foreach(chipmunk_target chipmunk chipmunk_static)
	if(TARGET ${chipmunk_target})
		target_compile_options(${chipmunk_target} PRIVATE -fno-fast-math)
	endif()
endforeach()

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
add_subdirectory(ska_scripting)

# Module boundaries (see cmake/CheckBoundaries.cmake), checked at each build
add_custom_target(check_boundaries ALL
	COMMAND ${CMAKE_COMMAND} -DSKA_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckBoundaries.cmake
	COMMENT "Checking the module boundaries")
# Only ska_scripting links skalang
foreach(ENGINE_MODULE ska_ai ska_audio ska_core ska_draw ska_gui ska_main ska_particle ska_physic_chipmunk ska_physic_homemade ska_script ska_world)
	get_target_property(ENGINE_MODULE_LIBRARIES ${ENGINE_MODULE} LINK_LIBRARIES)
	foreach(LIBRARY ${ENGINE_MODULE_LIBRARIES})
		if(LIBRARY MATCHES "^skalang|^ska_scripting$")
			message(FATAL_ERROR "Module boundaries violated : ${ENGINE_MODULE} links ${LIBRARY} (the engine modules do not depend on the scripting)")
		endif()
	endforeach()
endforeach()


