#version 310 es

precision mediump float;

layout(location = 0) in vec3 v_color;

layout(location = 0) out vec4 FragColor;

void main() {
	vec3 color = v_color * 0.5 + 0.5;

	FragColor = vec4(color, 1.0);
}