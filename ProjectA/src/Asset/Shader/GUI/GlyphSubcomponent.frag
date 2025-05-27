#version 440

const float ALPHA_THRESHOLD = 0.5;
const float SMOOTHNESS = 0.05;

in VertexOutput {
	layout(location = 0) vec3 imageCoordinate;
	layout(location = 1) vec4 color;
} vertexOutput;

// TODO: Replace with sampler2DArray for multiple font glyph maps.
layout(set = 0, binding = 1) uniform sampler2D glyphMapSampler;

layout(location = 0) out vec4 color;

void main() {
	float value = smoothstep(
		ALPHA_THRESHOLD - SMOOTHNESS,
		ALPHA_THRESHOLD + SMOOTHNESS,
		texture(glyphMapSampler, vertexOutput.imageCoordinate.xy).r
	);

	color = vec4(value) * vertexOutput.color;
}