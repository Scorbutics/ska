# A shader of the ska renderer (GLSL 450, the layout of Graphic/Shader.h) as GLSL ES 1.00 for WebGL (the web renderer :
# Shader.cpp loads "<path>.frag.es"). Run as a script : cmake -DINPUT=<file.frag> -DOUTPUT=<file.frag.es> -P SkaShaderEs.cmake
#
#   in (location 0, 1)          -> varying v_color, v_uv
#   out o_color                 -> gl_FragColor
#   the texture (set 2)         -> uniform sampler2D u_texture, and uniform vec2 u_textureSize (textureSize(u_texture, 0))
#   the uniform block (set 3)   -> a uniform per member ; its members listed in a first line "// ska-uniforms <type> <name>..."
#                                  (the renderer gives them from the bytes of the block, std140 : float, vec2, vec3, vec4)
#   texture(...)                -> texture2D(...)

file(READ "${INPUT}" source)

set(members "")
if(source MATCHES "layout\\(set = 3, binding = 0\\) uniform [A-Za-z_0-9]+ {([^}]*)};")
	set(block "${CMAKE_MATCH_1}")
	set(uniforms "")
	string(REGEX MATCHALL "(float|vec2|vec3|vec4|int) [A-Za-z_0-9]+;" declarations "${block}")
	foreach(declaration ${declarations})
		string(REGEX REPLACE ";$" "" declaration "${declaration}")
		string(APPEND members " ${declaration}")
		string(APPEND uniforms "uniform ${declaration};\n")
	endforeach()
	string(REGEX REPLACE "layout\\(set = 3, binding = 0\\) uniform [A-Za-z_0-9]+ {[^}]*};" "${uniforms}" source "${source}")
endif()

string(REGEX REPLACE "#version 450[^\n]*\n" "" source "${source}")
string(REGEX REPLACE "layout\\(location = [0-9]+\\) in " "varying " source "${source}")
string(REGEX REPLACE "layout\\(location = [0-9]+\\) out vec4 o_color;[^\n]*\n" "" source "${source}")
string(REPLACE "layout(set = 2, binding = 0) uniform sampler2D u_texture;" "uniform sampler2D u_texture;\nuniform vec2 u_textureSize;" source "${source}")
string(REGEX REPLACE "textureSize\\(u_texture, *0\\)" "u_textureSize" source "${source}")
string(REGEX REPLACE "([^A-Za-z0-9_])texture\\(" "\\1texture2D(" source "${source}")
string(REPLACE "o_color" "gl_FragColor" source "${source}")

file(WRITE "${OUTPUT}" "// ska-uniforms${members}\n#version 100\nprecision highp float;\n${source}")
