project(ska_gui)

add_subdirectory(src)
if(${SKA_BUILD_NO_TEST})
else()
	add_subdirectory(test)
endif()
