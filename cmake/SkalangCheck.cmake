# Static check of skalang scripts in a build : compiles them (parsing, type checking, bytecode generation) without running them.
#
# skalang_check_scripts(<target>
#     SCRIPTS <file>...                  scripts declaring "run = function(args: string[]) do ... end"
#     [MODULES <file>...]                scripts only imported by other scripts
#     [BIND_DIRECTORIES <directory>...]  each <name>.miniska file is the template of "bind:<name>" (the functions bound by the host program)
#     [WORKING_DIRECTORY <directory>])   directory of the "wd:" imports
#
# Creates the <target> target, built with "all" : the build fails with the compilation errors of the scripts.
# The check runs again when a script or a template changes.
# The --bind arguments of the templates of the directories (a template of a later directory overrides one of an earlier directory)
function(_skalang_bind_arguments ARGUMENTS_VARIABLE TEMPLATES_VARIABLE)
	set(ARGUMENTS "")
	set(ALL_TEMPLATES "")
	foreach(BIND_DIRECTORY ${ARGN})
		file(GLOB TEMPLATES CONFIGURE_DEPENDS ${BIND_DIRECTORY}/*.miniska)
		foreach(TEMPLATE ${TEMPLATES})
			get_filename_component(BIND_NAME ${TEMPLATE} NAME_WLE)
			list(APPEND ARGUMENTS --bind "${BIND_NAME}=${TEMPLATE}")
			list(APPEND ALL_TEMPLATES ${TEMPLATE})
		endforeach()
	endforeach()
	set(${ARGUMENTS_VARIABLE} ${ARGUMENTS} PARENT_SCOPE)
	set(${TEMPLATES_VARIABLE} ${ALL_TEMPLATES} PARENT_SCOPE)
endfunction()

function(skalang_check_scripts TARGET_NAME)
	cmake_parse_arguments(CHECK "" "WORKING_DIRECTORY" "SCRIPTS;MODULES;BIND_DIRECTORIES" ${ARGN})
	if(NOT TARGET skalang_check)
		message(FATAL_ERROR "skalang_check_scripts : the skalang_check executable is not built (SKALANG_BUILD_CHECKER is OFF)")
	endif()

	set(CHECK_ARGUMENTS "")
	set(CHECK_DEPENDENCIES ${CHECK_SCRIPTS} ${CHECK_MODULES})
	_skalang_bind_arguments(BIND_ARGUMENTS TEMPLATES ${CHECK_BIND_DIRECTORIES})
	list(APPEND CHECK_ARGUMENTS ${BIND_ARGUMENTS})
	list(APPEND CHECK_DEPENDENCIES ${TEMPLATES})
	if(CHECK_WORKING_DIRECTORY)
		list(APPEND CHECK_ARGUMENTS --wd ${CHECK_WORKING_DIRECTORY})
	endif()
	foreach(MODULE ${CHECK_MODULES})
		list(APPEND CHECK_ARGUMENTS --module ${MODULE})
	endforeach()
	list(APPEND CHECK_ARGUMENTS ${CHECK_SCRIPTS})

	set(STAMP ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.stamp)
	add_custom_command(OUTPUT ${STAMP}
		COMMAND $<TARGET_FILE:skalang_check> ${CHECK_ARGUMENTS}
		COMMAND ${CMAKE_COMMAND} -E touch ${STAMP}
		DEPENDS skalang_check ${CHECK_DEPENDENCIES}
		COMMENT "Checking the skalang scripts (${TARGET_NAME})"
		VERBATIM)
	add_custom_target(${TARGET_NAME} ALL DEPENDS ${STAMP})
endfunction()

# Headless tests of scripts (ctest) : each test script is compiled and run by "skalang_check --run".
#
# skalang_test_scripts(<test name prefix>
#     TESTS <file>...                    test scripts : "run = function(args: string[]) do ... end", with the assertions of
#                                        Test = import "bind:std.native.test" (a failed assertion or an error fails the test)
#     [BIND_DIRECTORIES <directory>...]  each <name>.miniska file is the fake of "bind:<name>" : the templates of the bindings
#                                        (their default bodies), or test fakes in a later directory (they override them)
#     [WORKING_DIRECTORY <directory>])   directory of the "wd:" imports
#
# The project calls enable_testing() ; the tests are run with ctest (no window : the scripts run without the host program).
function(skalang_test_scripts PREFIX)
	cmake_parse_arguments(TESTS "" "WORKING_DIRECTORY" "TESTS;BIND_DIRECTORIES" ${ARGN})
	if(NOT TARGET skalang_check)
		message(FATAL_ERROR "skalang_test_scripts : the skalang_check executable is not built (SKALANG_BUILD_CHECKER is OFF)")
	endif()
	_skalang_bind_arguments(BIND_ARGUMENTS TEMPLATES ${TESTS_BIND_DIRECTORIES})
	set(TEST_ARGUMENTS --run ${BIND_ARGUMENTS})
	if(TESTS_WORKING_DIRECTORY)
		list(APPEND TEST_ARGUMENTS --wd ${TESTS_WORKING_DIRECTORY})
	endif()
	foreach(TEST_SCRIPT ${TESTS_TESTS})
		get_filename_component(TEST_NAME ${TEST_SCRIPT} NAME_WE)
		add_test(NAME ${PREFIX}_${TEST_NAME} COMMAND skalang_check ${TEST_ARGUMENTS} ${TEST_SCRIPT})
	endforeach()
endfunction()
