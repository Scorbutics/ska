# ccache as the compiler launcher, when it is installed and no launcher is given (-DCMAKE_CXX_COMPILER_LAUNCHER=...) :
# a file compiled before with the same flags and the same headers is taken from its cache (a new build folder, another
# branch, a configuration change). Included by the engine and by the games, before their targets.
find_program(SKA_CCACHE ccache)
if(SKA_CCACHE)
	foreach(language C CXX)
		if(NOT CMAKE_${language}_COMPILER_LAUNCHER)
			set(CMAKE_${language}_COMPILER_LAUNCHER ${SKA_CCACHE})
		endif()
	endforeach()
endif()
