#version 440

precision mediump float;

in VertexOutput {
	layout(location = 0) vec3 imageCoordinate;
	layout(location = 1) vec4 color;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2D imageSamplers[2];

layout(location = 0) out vec4 color;

void main() {
	uint imageIndex = uint(vertexOutput.imageCoordinate.z);
	vec4 imageColor;

	// See https://stackoverflow.com/a/72046722/17136841.
	switch (imageIndex) {
		case 0:
			imageColor = texture(imageSamplers[0], vertexOutput.imageCoordinate.xy) * vertexOutput.color;

			break;
		case 1:
			imageColor = texture(imageSamplers[1], vertexOutput.imageCoordinate.xy) * vertexOutput.color;

			break;
		default:
			break;
	}

	color = imageColor;
}