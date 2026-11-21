# Tests of skalang_check (run with ctest) and of skalang_check_scripts (checked during the build)
set(CHECKER_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CHECKER_BIND --bind test.template=${CHECKER_TEST_DIR}/bind/test.template.miniska)
set(CHECKER_FAILED "1 / 1 script\\(s\\) failed to compile")

# A script using a binding template, a "wd:" module, and the module itself
add_test(NAME skalang_check_valid_scripts
	COMMAND skalang_check ${CHECKER_BIND} --wd ${CHECKER_TEST_DIR}
		--module ${CHECKER_TEST_DIR}/sub/helper.miniska ${CHECKER_TEST_DIR}/ok_script.miniska)

# The exit code is the number of scripts that failed
add_test(NAME skalang_check_exit_code_on_error
	COMMAND skalang_check ${CHECKER_TEST_DIR}/type_error.miniska)
set_tests_properties(skalang_check_exit_code_on_error PROPERTIES WILL_FAIL TRUE)

add_test(NAME skalang_check_type_error
	COMMAND skalang_check ${CHECKER_TEST_DIR}/type_error.miniska)
set_tests_properties(skalang_check_type_error PROPERTIES
	PASS_REGULAR_EXPRESSION "type_error.miniska.*Unable to use operator \"\\+\".*no implicit conversion.*${CHECKER_FAILED}")

# The functions of a binding are checked against its template
add_test(NAME skalang_check_wrong_binding_call
	COMMAND skalang_check ${CHECKER_BIND} ${CHECKER_TEST_DIR}/wrong_binding_call.miniska)
set_tests_properties(skalang_check_wrong_binding_call PROPERTIES
	PASS_REGULAR_EXPRESSION "Unable to use operator \"=\" on types \"int\" and \"string\".*${CHECKER_FAILED}")

# A script declares a run function, unless it is checked as a module
add_test(NAME skalang_check_script_without_run
	COMMAND skalang_check ${CHECKER_TEST_DIR}/no_run.miniska)
set_tests_properties(skalang_check_script_without_run PROPERTIES
	PASS_REGULAR_EXPRESSION "a script declares \"run = function\\(args: string\\[\\]\\)")
add_test(NAME skalang_check_module_without_run
	COMMAND skalang_check --module ${CHECKER_TEST_DIR}/no_run.miniska)

add_test(NAME skalang_check_unknown_binding
	COMMAND skalang_check ${CHECKER_TEST_DIR}/unknown_binding.miniska)
set_tests_properties(skalang_check_unknown_binding PROPERTIES
	PASS_REGULAR_EXPRESSION "needs \"--bind <name>=<template file>\"")

add_test(NAME skalang_check_missing_file
	COMMAND skalang_check ${CHECKER_TEST_DIR}/does_not_exist.miniska)
set_tests_properties(skalang_check_missing_file PROPERTIES
	PASS_REGULAR_EXPRESSION "does_not_exist.miniska : file not found")

# All the files are checked : each failure is counted
add_test(NAME skalang_check_counts_failures
	COMMAND skalang_check ${CHECKER_TEST_DIR}/type_error.miniska ${CHECKER_TEST_DIR}/no_run.miniska --module ${CHECKER_TEST_DIR}/no_run.miniska)
set_tests_properties(skalang_check_counts_failures PROPERTIES
	PASS_REGULAR_EXPRESSION "2 / 3 script\\(s\\) failed to compile")

add_test(NAME skalang_check_usage
	COMMAND skalang_check --unknown-option)
set_tests_properties(skalang_check_usage PROPERTIES WILL_FAIL TRUE)
add_test(NAME skalang_check_no_file
	COMMAND skalang_check)
set_tests_properties(skalang_check_no_file PROPERTIES WILL_FAIL TRUE)

# The CMake helper : the valid scripts are checked during the build
skalang_check_scripts(skalang_checker_test_scripts
	SCRIPTS ${CHECKER_TEST_DIR}/ok_script.miniska
	MODULES ${CHECKER_TEST_DIR}/sub/helper.miniska
	BIND_DIRECTORIES ${CHECKER_TEST_DIR}/bind
	WORKING_DIRECTORY ${CHECKER_TEST_DIR})

# Headless test scripts (skalang_test_scripts) : templates as fakes, assertions
skalang_test_scripts(skalang_script_defaults
	TESTS ${CHECKER_TEST_DIR}/tests/defaults_test.miniska ${CHECKER_TEST_DIR}/tests/failing_test.miniska
	BIND_DIRECTORIES ${CHECKER_TEST_DIR}/bind
	WORKING_DIRECTORY ${CHECKER_TEST_DIR})
set_tests_properties(skalang_script_defaults_failing_test PROPERTIES WILL_FAIL TRUE)
skalang_test_scripts(skalang_script_fakes
	TESTS ${CHECKER_TEST_DIR}/tests/fake_test.miniska
	BIND_DIRECTORIES ${CHECKER_TEST_DIR}/bind ${CHECKER_TEST_DIR}/fakes_dir)
add_test(NAME skalang_check_run_failure_message
	COMMAND skalang_check --run ${CHECKER_TEST_DIR}/tests/failing_test.miniska)
set_tests_properties(skalang_check_run_failure_message PROPERTIES
	PASS_REGULAR_EXPRESSION "equal failed : a failing assertion ..1. instead of .2...*1 / 1 script.s. failed to compile or run")
