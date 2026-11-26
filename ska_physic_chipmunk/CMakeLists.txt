project(ska_physic_chipmunk)

add_subdirectory(src)
# A desktop demo (not on the web ; SKA_BUILD_DEMOS, the engine built alone)
if(NOT EMSCRIPTEN AND SKA_BUILD_DEMOS)
	add_subdirectory(simulation)
endif()

