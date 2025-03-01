#version 450 core

layout(location = 0) in vec2 v_uv;

layout(location = 0) out vec4 color;

void main() {
	color = vec4(v_uv * 0.5 + 0.5, 0.0, 1.0);
}