#version 450

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 textureCoordinate;

layout(set = 0, binding = 0) uniform Camera {
	mat4x4 projection;
} camera;

out VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

void main() {
	gl_Position = camera.projection * vec4(a_position, 0.0, 1.0);

	vertexOutput.textureCoordinate = textureCoordinate;
}