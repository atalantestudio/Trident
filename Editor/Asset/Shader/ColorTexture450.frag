#version 450 core

precision mediump float;

struct VertexOutput {
	vec4 color;
	vec2 uv;
	int textureSlot;
	float tilingFactor;
};

layout(location = 0) flat in VertexOutput fragment_input;

layout(/*set = 1, */binding = 0) uniform sampler2D u_textures[32];

layout(location = 0) out vec4 color;

void main() {
	vec4 textureColor = fragment_input.color;

	switch(fragment_input.textureSlot) {
		case  0: textureColor *= texture(u_textures[0], fragment_input.uv * fragment_input.tilingFactor); break;
		case  1: textureColor *= texture(u_textures[1], fragment_input.uv * fragment_input.tilingFactor); break;
		case  2: textureColor *= texture(u_textures[2], fragment_input.uv * fragment_input.tilingFactor); break;
		case  3: textureColor *= texture(u_textures[3], fragment_input.uv * fragment_input.tilingFactor); break;
		case  4: textureColor *= texture(u_textures[4], fragment_input.uv * fragment_input.tilingFactor); break;
		case  5: textureColor *= texture(u_textures[5], fragment_input.uv * fragment_input.tilingFactor); break;
		case  6: textureColor *= texture(u_textures[6], fragment_input.uv * fragment_input.tilingFactor); break;
		case  7: textureColor *= texture(u_textures[7], fragment_input.uv * fragment_input.tilingFactor); break;
		case  8: textureColor *= texture(u_textures[8], fragment_input.uv * fragment_input.tilingFactor); break;
		case  9: textureColor *= texture(u_textures[9], fragment_input.uv * fragment_input.tilingFactor); break;
		case 10: textureColor *= texture(u_textures[10], fragment_input.uv * fragment_input.tilingFactor); break;
		case 11: textureColor *= texture(u_textures[11], fragment_input.uv * fragment_input.tilingFactor); break;
		case 12: textureColor *= texture(u_textures[12], fragment_input.uv * fragment_input.tilingFactor); break;
		case 13: textureColor *= texture(u_textures[13], fragment_input.uv * fragment_input.tilingFactor); break;
		case 14: textureColor *= texture(u_textures[14], fragment_input.uv * fragment_input.tilingFactor); break;
		case 15: textureColor *= texture(u_textures[15], fragment_input.uv * fragment_input.tilingFactor); break;
		case 16: textureColor *= texture(u_textures[16], fragment_input.uv * fragment_input.tilingFactor); break;
		case 17: textureColor *= texture(u_textures[17], fragment_input.uv * fragment_input.tilingFactor); break;
		case 18: textureColor *= texture(u_textures[18], fragment_input.uv * fragment_input.tilingFactor); break;
		case 19: textureColor *= texture(u_textures[19], fragment_input.uv * fragment_input.tilingFactor); break;
		case 20: textureColor *= texture(u_textures[20], fragment_input.uv * fragment_input.tilingFactor); break;
		case 21: textureColor *= texture(u_textures[21], fragment_input.uv * fragment_input.tilingFactor); break;
		case 22: textureColor *= texture(u_textures[22], fragment_input.uv * fragment_input.tilingFactor); break;
		case 23: textureColor *= texture(u_textures[23], fragment_input.uv * fragment_input.tilingFactor); break;
		case 24: textureColor *= texture(u_textures[24], fragment_input.uv * fragment_input.tilingFactor); break;
		case 25: textureColor *= texture(u_textures[25], fragment_input.uv * fragment_input.tilingFactor); break;
		case 26: textureColor *= texture(u_textures[26], fragment_input.uv * fragment_input.tilingFactor); break;
		case 27: textureColor *= texture(u_textures[27], fragment_input.uv * fragment_input.tilingFactor); break;
		case 28: textureColor *= texture(u_textures[28], fragment_input.uv * fragment_input.tilingFactor); break;
		case 29: textureColor *= texture(u_textures[29], fragment_input.uv * fragment_input.tilingFactor); break;
		case 30: textureColor *= texture(u_textures[30], fragment_input.uv * fragment_input.tilingFactor); break;
		case 31: textureColor *= texture(u_textures[31], fragment_input.uv * fragment_input.tilingFactor); break;
	}

	color = textureColor;
}