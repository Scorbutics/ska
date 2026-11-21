# Shaders of the ska renderer (Graphic/Shader.h) : GLSL fragment shaders compiled before the game runs.
#
# ska_compile_shaders(<target> OUTPUT_DIRECTORY <dir> SOURCES <file.frag>...)
#   Compiles each GLSL fragment shader to SPIR-V (Vulkan, the renderer on Linux and Android) with glslangValidator :
#   <dir>/<name>.frag.spv. A Shader loads it with the path "<dir>/<name>" (without ".frag.spv").
#   The target is built with the default target (ALL).
#   Metal (".frag.msl") and Direct3D 12 (".frag.dxil") need SDL_shadercross : not done here.
#
# The GLSL shader receives the color of the drawing (location 0), its texture coordinates (location 1), the texture
# (set 2, binding 0), and the uniforms of the shader (set 3, binding 0) when it has some :
#   #version 450
#   layout(location = 0) in vec4 v_color;
#   layout(location = 1) in vec2 v_uv;
#   layout(location = 0) out vec4 o_color;
#   layout(set = 2, binding = 0) uniform sampler2D u_texture;
#   void main() { o_color = texture(u_texture, v_uv) * v_color; }

find_program(SKA_GLSLANG_VALIDATOR glslangValidator)

function(ska_compile_shaders TARGET)
	cmake_parse_arguments(ARG "" "OUTPUT_DIRECTORY" "SOURCES" ${ARGN})
	if(NOT SKA_GLSLANG_VALIDATOR)
		message(WARNING "glslangValidator not found : the shaders of ${TARGET} are not compiled (the drawings are done without them)")
		add_custom_target(${TARGET})
		return()
	endif()
	set(outputs)
	foreach(source ${ARG_SOURCES})
		get_filename_component(source ${source} ABSOLUTE)
		get_filename_component(name ${source} NAME)
		set(output ${ARG_OUTPUT_DIRECTORY}/${name}.spv)
		add_custom_command(OUTPUT ${output}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${ARG_OUTPUT_DIRECTORY}
			COMMAND ${SKA_GLSLANG_VALIDATOR} -V ${source} -o ${output}
			DEPENDS ${source}
			COMMENT "Compiling the shader ${name}")
		list(APPEND outputs ${output})
	endforeach()
	add_custom_target(${TARGET} ALL DEPENDS ${outputs})
endfunction()
