cmake_minimum_required(VERSION 3.10)
project(skalang)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# skalang is either built alone, or embedded in a host project (add_subdirectory) : in that case,
# the host output directories and build type are kept, and the tests and the runner are not built by default.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(SKALANG_STANDALONE ON)
else()
	set(SKALANG_STANDALONE OFF)
endif()
option(SKALANG_BUILD_TESTS "Builds the skalang tests" ${SKALANG_STANDALONE})
option(SKALANG_BUILD_RUNNER "Builds the skalang script runner executable" ${SKALANG_STANDALONE})
option(SKALANG_BUILD_CHECKER "Builds skalang_check, the executable compiling scripts without running them (static check in a build)" ON)

if(SKALANG_STANDALONE)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if(SKALANG_STANDALONE AND NOT CMAKE_BUILD_TYPE)
  set( CMAKE_BUILD_TYPE Debug CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel."
       FORCE )
endif()

# skalog and skabase : next to skalang in the ska engine (external/skalog, external/skabase) ; the host ones are used when
# the host already provides them (same targets names). skabase sources need their own include directory, given by the
# including directory
if(NOT TARGET skalog)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../skalog/src/src)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../skalog/src skalog)
endif()
if(NOT TARGET skabase)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../skabase/src/src)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../skabase/src skabase)
endif()
get_target_property(SKA_LOG_TARGET_DIR skalog SOURCE_DIR)
get_target_property(SKA_BASE_TARGET_DIR skabase SOURCE_DIR)
set(SKA_LOG_DIR ${SKA_LOG_TARGET_DIR}/src)
set(SKA_BASE_DIR ${SKA_BASE_TARGET_DIR}/src)

set(SKALANG_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(SKALANG_BYTECODE_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/bytecode)
set(SKALANG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SKALANG_STD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/std)
set(SKALANG_BYTECODE_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bytecode/generator)
set(SKALANG_BYTECODE_SER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bytecode/serializer)
set(SKALANG_BYTECODE_INT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bytecode/interpreter)
set(SKALANG_BYTECODE_RUN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bytecode/runner)
set(SKALANG_BYTECODE_CHECK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bytecode/checker)

string( TOLOWER "${CMAKE_BUILD_TYPE}" build_type_lower )
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND build_type_lower STREQUAL "debug")
    MESSAGE("Clang compiler detected in debug mode. Using -fstandalone-debug flag.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstandalone-debug")
endif()

include_directories(${SKA_BASE_DIR})
include_directories(${SKA_LOG_DIR})
include_directories(src/src)


add_subdirectory(${SKALANG_SRC_DIR})
add_subdirectory(${SKALANG_BYTECODE_GEN_DIR})
add_subdirectory(${SKALANG_BYTECODE_SER_DIR})
add_subdirectory(${SKALANG_BYTECODE_INT_DIR})
add_subdirectory(${SKALANG_STD_DIR})
if(SKALANG_BUILD_TESTS)
	add_subdirectory(external/doctest)
	add_subdirectory(${SKALANG_TEST_DIR})
endif()
if(SKALANG_BUILD_RUNNER)
	add_subdirectory(${SKALANG_BYTECODE_RUN_DIR})
endif()
if(SKALANG_BUILD_CHECKER)
	add_subdirectory(${SKALANG_BYTECODE_CHECK_DIR})
	# skalang_check_scripts(...) : static check of scripts in a build
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SkalangCheck.cmake)
	if(SKALANG_BUILD_TESTS)
		# skalang_check tests : ctest
		enable_testing()
		add_subdirectory(test/checker)
	endif()
endif()

