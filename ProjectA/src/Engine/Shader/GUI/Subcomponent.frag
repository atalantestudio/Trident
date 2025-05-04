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

	color = texture(imageSamplers[imageIndex], vertexOutput.imageCoordinate.xy) * vertexOutput.color;
}