#version 450
// Test shader : the drawing takes the color given in the uniforms
layout(location = 0) in vec4 v_color;
layout(location = 1) in vec2 v_uv;
layout(location = 0) out vec4 o_color;
layout(set = 2, binding = 0) uniform sampler2D u_texture;
layout(set = 3, binding = 0) uniform Uniforms {
	vec4 u_color;
};

void main() {
	vec4 color = texture(u_texture, v_uv) * v_color;
	o_color = vec4(u_color.rgb, color.a);
}
