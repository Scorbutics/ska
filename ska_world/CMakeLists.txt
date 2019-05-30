project(ska_world)

add_subdirectory(src)
if(${SKA_BUILD_NO_TEST})
else()
	add_subdirectory(test)
endif()
