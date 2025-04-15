#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in int a_textureSlot;
layout(location = 4) in float a_tilingFactor;

uniform mat4 u_projectionView;

out vec4 v_color;
out vec2 v_uv;
flat out int v_textureSlot;
out float v_tilingFactor;

void main() {
	gl_Position = vec4(a_position, 1.0) * u_projectionView;

	v_color = a_color;
	v_uv = a_uv;
	v_textureSlot = int(a_textureSlot);
	v_tilingFactor = a_tilingFactor;
}