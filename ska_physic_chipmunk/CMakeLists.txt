project(ska_physic_chipmunk)

add_subdirectory(src)
# A desktop demo (not on the web)
if(NOT EMSCRIPTEN)
	add_subdirectory(simulation)
endif()

