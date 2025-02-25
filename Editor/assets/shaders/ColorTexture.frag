#version 310 es

precision mediump float;

layout(location = 0) in vec4 v_color;
layout(location = 1) in vec2 v_uv;
layout(location = 2) flat in int v_textureSlot;
layout(location = 3) in float v_tilingFactor;

layout(binding = 0) uniform sampler2D u_textures[32];

layout(location = 0) out vec4 color;

void main() {
	vec4 textureColor = v_color;

	switch(v_textureSlot) {
		case  0: textureColor *= texture(u_textures[0], v_uv * v_tilingFactor); break;
		case  1: textureColor *= texture(u_textures[1], v_uv * v_tilingFactor); break;
		case  2: textureColor *= texture(u_textures[2], v_uv * v_tilingFactor); break;
		case  3: textureColor *= texture(u_textures[3], v_uv * v_tilingFactor); break;
		case  4: textureColor *= texture(u_textures[4], v_uv * v_tilingFactor); break;
		case  5: textureColor *= texture(u_textures[5], v_uv * v_tilingFactor); break;
		case  6: textureColor *= texture(u_textures[6], v_uv * v_tilingFactor); break;
		case  7: textureColor *= texture(u_textures[7], v_uv * v_tilingFactor); break;
		case  8: textureColor *= texture(u_textures[8], v_uv * v_tilingFactor); break;
		case  9: textureColor *= texture(u_textures[9], v_uv * v_tilingFactor); break;
		case 10: textureColor *= texture(u_textures[10], v_uv * v_tilingFactor); break;
		case 11: textureColor *= texture(u_textures[11], v_uv * v_tilingFactor); break;
		case 12: textureColor *= texture(u_textures[12], v_uv * v_tilingFactor); break;
		case 13: textureColor *= texture(u_textures[13], v_uv * v_tilingFactor); break;
		case 14: textureColor *= texture(u_textures[14], v_uv * v_tilingFactor); break;
		case 15: textureColor *= texture(u_textures[15], v_uv * v_tilingFactor); break;
		case 16: textureColor *= texture(u_textures[16], v_uv * v_tilingFactor); break;
		case 17: textureColor *= texture(u_textures[17], v_uv * v_tilingFactor); break;
		case 18: textureColor *= texture(u_textures[18], v_uv * v_tilingFactor); break;
		case 19: textureColor *= texture(u_textures[19], v_uv * v_tilingFactor); break;
		case 20: textureColor *= texture(u_textures[20], v_uv * v_tilingFactor); break;
		case 21: textureColor *= texture(u_textures[21], v_uv * v_tilingFactor); break;
		case 22: textureColor *= texture(u_textures[22], v_uv * v_tilingFactor); break;
		case 23: textureColor *= texture(u_textures[23], v_uv * v_tilingFactor); break;
		case 24: textureColor *= texture(u_textures[24], v_uv * v_tilingFactor); break;
		case 25: textureColor *= texture(u_textures[25], v_uv * v_tilingFactor); break;
		case 26: textureColor *= texture(u_textures[26], v_uv * v_tilingFactor); break;
		case 27: textureColor *= texture(u_textures[27], v_uv * v_tilingFactor); break;
		case 28: textureColor *= texture(u_textures[28], v_uv * v_tilingFactor); break;
		case 29: textureColor *= texture(u_textures[29], v_uv * v_tilingFactor); break;
		case 30: textureColor *= texture(u_textures[30], v_uv * v_tilingFactor); break;
		case 31: textureColor *= texture(u_textures[31], v_uv * v_tilingFactor); break;
	}

	color = textureColor;
}