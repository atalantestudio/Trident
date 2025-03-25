#version 320 es

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in mat3x3 world;
layout(location = 4) in uint colorIndex;
layout(location = 5) in uint textureIndex;

out VertexOutput {
	layout(location = 0) flat uint colorIndex;
	layout(location = 1) flat uint textureIndex;
} vertexOutput;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

void main() {
	gl_Position = vec4(camera.projection * world * vertexPosition, 1.0);

	vertexOutput.colorIndex = colorIndex;
	vertexOutput.textureIndex = textureIndex;
}