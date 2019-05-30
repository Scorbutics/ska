project(ska_particle)

add_subdirectory(src)

if(${SKA_BUILD_NO_TEST})
else()
	add_subdirectory(test)
	add_subdirectory(benchmark)
endif()

