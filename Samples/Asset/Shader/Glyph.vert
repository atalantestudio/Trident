#version 450

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 textureCoordinate;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

out VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

void main() {
	gl_Position = vec4(camera.projection * vec3(vertex, 1.0), 1.0);

	vertexOutput.textureCoordinate = textureCoordinate;
}