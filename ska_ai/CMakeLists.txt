cmake_minimum_required(VERSION 3.9)
project(ska_ai)

add_subdirectory(src)
if(${SKA_BUILD_NO_TEST})
else()
	add_subdirectory(test)
endif()
