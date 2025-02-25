#version 450 core

struct VertexOutput {
	vec4 color;
	vec2 uv;
	int textureSlot;
	float tilingFactor;
};

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in int a_textureSlot;
layout(location = 4) in float a_tilingFactor;

layout(std140, binding = 0) uniform Camera {
	mat4 u_projectionView;
};

layout(location = 0) out VertexOutput vertex_output;

void main() {
	gl_Position = vec4(a_position, 1.0) * u_projectionView;

	vertex_output.color = a_color;
	vertex_output.uv = a_uv;
	vertex_output.textureSlot = int(a_textureSlot);
	vertex_output.tilingFactor = a_tilingFactor;
}