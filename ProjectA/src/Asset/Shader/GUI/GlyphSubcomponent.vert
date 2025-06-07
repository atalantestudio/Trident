#version 440

// TODO: Remove padding.
layout(location = 0) in vec4 vertexPosition;

layout(location = 1) in vec2 vertexImageCoordinate;
layout(location = 2) in mat3x3 world;
layout(location = 5) in mat3x3 imageWorld;
layout(location = 8) in uint fontIndex;
layout(location = 9) in float outlineWidth;
layout(location = 10) in vec4 color;
layout(location = 11) in vec4 outlineColor;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

out VertexOutput {
	layout(location = 0) vec3 imageCoordinate;
	layout(location = 1) float outlineWidth;
	layout(location = 2) vec4 color;
	layout(location = 3) vec4 outlineColor;
} vertexOutput;

void main() {
	gl_Position = vec4(camera.projection * world * vertexPosition.xyz, 1.0);

	vertexOutput.imageCoordinate = imageWorld * vec3(vertexImageCoordinate, 1.0);
	vertexOutput.imageCoordinate.z = float(fontIndex);
	vertexOutput.outlineWidth = outlineWidth;
	vertexOutput.color = color;
	vertexOutput.outlineColor = outlineColor;
}