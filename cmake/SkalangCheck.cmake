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
function(skalang_check_scripts TARGET_NAME)
	cmake_parse_arguments(CHECK "" "WORKING_DIRECTORY" "SCRIPTS;MODULES;BIND_DIRECTORIES" ${ARGN})
	if(NOT TARGET skalang_check)
		message(FATAL_ERROR "skalang_check_scripts : the skalang_check executable is not built (SKALANG_BUILD_CHECKER is OFF)")
	endif()

	set(CHECK_ARGUMENTS "")
	set(CHECK_DEPENDENCIES ${CHECK_SCRIPTS} ${CHECK_MODULES})
	foreach(BIND_DIRECTORY ${CHECK_BIND_DIRECTORIES})
		file(GLOB TEMPLATES CONFIGURE_DEPENDS ${BIND_DIRECTORY}/*.miniska)
		foreach(TEMPLATE ${TEMPLATES})
			get_filename_component(BIND_NAME ${TEMPLATE} NAME_WLE)
			list(APPEND CHECK_ARGUMENTS --bind "${BIND_NAME}=${TEMPLATE}")
			list(APPEND CHECK_DEPENDENCIES ${TEMPLATE})
		endforeach()
	endforeach()
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
