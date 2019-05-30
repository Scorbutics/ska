project(ska_physic_homemade)

add_subdirectory(src)

if(${SKA_BUILD_NO_TEST})
else()
	add_subdirectory(simulation)
endif()
