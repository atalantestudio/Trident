#version 440

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexTextureCoordinates;
layout(location = 2) in mat3x3 world;
layout(location = 5) in vec4 color;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

out VertexOutput {
	layout(location = 0) vec4 color;
} vertexOutput;

void main() {
	gl_Position = vec4(camera.projection * world * vertexPosition, 1.0);

	vertexOutput.color = color;
}