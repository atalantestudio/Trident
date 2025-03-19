#version 450

layout(location = 0) in vec2 v_uv;

layout(set = 0, binding = 0) uniform sampler2D u_sampler;

layout(location = 0) out vec4 color;

void main() {
	color = texture(u_sampler, v_uv);
}