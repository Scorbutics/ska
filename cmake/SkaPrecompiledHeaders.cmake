# Precompiled headers : the C++ standard headers, 80 to 90 % of the lines a source file of the engine, of skalang or of
# a game compiles. They never change : a precompiled header of them is never rebuilt by a change of the code.
# SKA_PRECOMPILED_HEADERS (on, but on the web : not checked there). A source missing one of these includes still compiles
# with it : the web build, without them, finds it.
if(EMSCRIPTEN)
	option(SKA_PRECOMPILED_HEADERS "The C++ standard headers precompiled" OFF)
else()
	option(SKA_PRECOMPILED_HEADERS "The C++ standard headers precompiled" ON)
endif()

set(SKA_PRECOMPILED_STD_HEADERS <string> <vector> <memory> <algorithm> <unordered_map> <unordered_set> <iostream>
	<fstream> <sstream> <cassert> <functional> <optional> <cmath> <array> <limits> <cstdint> <utility> <tuple> <map>
	<variant> <deque>)

# ska_precompile_std(<target>...) : the targets that exist (an optional module may be absent) and compile sources, their
# C++ sources only
function(ska_precompile_std)
	if(NOT SKA_PRECOMPILED_HEADERS)
		return()
	endif()
	foreach(target ${ARGN})
		if(TARGET ${target})
			get_target_property(type ${target} TYPE)
			if(type STREQUAL "INTERFACE_LIBRARY")
				continue()
			endif()
			foreach(header ${SKA_PRECOMPILED_STD_HEADERS})
				target_precompile_headers(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${header}>)
			endforeach()
		endif()
	endforeach()
endfunction()
