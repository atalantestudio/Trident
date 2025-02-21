#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in int a_textureSlot;
layout(location = 4) in float a_tilingFactor;

uniform mat4 u_viewProjection;

out vec4 v_color;
out vec2 v_uv;
flat out int v_textureSlot;
out float v_tilingFactor;

void main() {
	gl_Position = vec4(a_position, 1.0) * u_viewProjection;

	v_color = a_color;
	v_uv = a_uv;
	v_textureSlot = int(a_textureSlot);
	v_tilingFactor = a_tilingFactor;
}

#type fragment
#version 330 core

in vec4 v_color;
in vec2 v_uv;
flat in int v_textureSlot;
in float v_tilingFactor;

uniform sampler2D u_textures[32];

layout(location = 0) out vec4 color;

void main() {
	color = v_color * texture(u_textures[v_textureSlot], v_uv * v_tilingFactor);
}