cmake_minimum_required(VERSION 3.4.1)
project(ska)

file(GLOB CPP_FILES "*.cpp")

add_library(
     ska
     STATIC
     ${CPP_FILES}
)

include_directories(ska_ai)
include_directories(ska_audio)
include_directories(ska_core)
include_directories(ska_draw)
include_directories(ska_gui)
include_directories(ska_particle)
include_directories(ska_physic)
include_directories(ska_script)
include_directories(ska_world)
