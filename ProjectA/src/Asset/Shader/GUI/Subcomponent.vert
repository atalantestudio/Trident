#version 440

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexImageCoordinate;
layout(location = 2) in mat3x3 world;
layout(location = 5) in mat3x3 imageWorld;
layout(location = 8) in uint imageIndex;
layout(location = 9) in vec4 color;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

out VertexOutput {
	layout(location = 0) vec3 imageCoordinate;
	layout(location = 1) vec4 color;
} vertexOutput;

void main() {
	gl_Position = vec4(camera.projection * world * vertexPosition, 1.0);

	vertexOutput.imageCoordinate = imageWorld * vec3(vertexImageCoordinate, 1.0);
	vertexOutput.imageCoordinate.z = float(imageIndex);
	vertexOutput.color = color;
}