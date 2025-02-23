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