# Checks the boundaries between the engine modules and the scripting (cmake -P, run at each build) :
# - the engine modules know nothing about the scripting : they do not include skalang nor ska_scripting headers
#   (the scripts are started through the ScriptRunner interface of ska_core)
# - ska_scripting only uses ska_core among the engine modules
# Usage : cmake -DSKA_SOURCE_DIR=<ska root> -P CheckBoundaries.cmake

set(ENGINE_MODULES ska_ai ska_audio ska_core ska_draw ska_gui ska_main ska_particle ska_physic_chipmunk ska_physic_homemade ska_script ska_world)
# Include roots of skalang and of ska_scripting
set(SCRIPTING_INCLUDES "Scripting|Config|Container|Error|Event|NodeValue|Operation|Runtime|Service|Generator|BytecodeInterpreter|Serializer|std")
# Include roots of the engine modules other than ska_core
set(OTHER_MODULES_INCLUDES "AI|Audio|Graphic|GUI|Main|Particle|Physic|Script|World")

set(VIOLATIONS "")

function(check_includes MODULE FORBIDDEN REASON)
	file(GLOB_RECURSE SOURCES "${SKA_SOURCE_DIR}/${MODULE}/src/*.h" "${SKA_SOURCE_DIR}/${MODULE}/src/*.cpp")
	foreach(SOURCE ${SOURCES})
		file(STRINGS "${SOURCE}" INCLUDES REGEX "^[ \t]*#[ \t]*include[ \t]*[<\"](${FORBIDDEN})/")
		foreach(INCLUDE ${INCLUDES})
			file(RELATIVE_PATH RELATIVE_SOURCE "${SKA_SOURCE_DIR}" "${SOURCE}")
			string(STRIP "${INCLUDE}" INCLUDE)
			set(VIOLATIONS "${VIOLATIONS}\n  ${RELATIVE_SOURCE} : ${INCLUDE} (${REASON})" PARENT_SCOPE)
		endforeach()
	endforeach()
endfunction()

foreach(MODULE ${ENGINE_MODULES})
	check_includes(${MODULE} "${SCRIPTING_INCLUDES}" "the engine modules do not depend on the scripting : use the ScriptRunner interface")
endforeach()
check_includes(ska_scripting "${OTHER_MODULES_INCLUDES}" "ska_scripting only depends on ska_core : bind the other modules in the game")

if(VIOLATIONS)
	message(FATAL_ERROR "Module boundaries violated :${VIOLATIONS}")
endif()
message(STATUS "Module boundaries checked")
