#version 440

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 imageCoordinate;
layout(location = 2) in mat3x3 world;
layout(location = 5) in mat3x3 imageWorld;
layout(location = 8) in uint layerIndex;
layout(location = 9) in vec4 color;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

out VertexOutput {
	layout(location = 0) vec3 textureCoordinate;
	layout(location = 1) vec4 color;
} vertexOutput;

void main() {
	gl_Position = vec4(camera.projection * world * vec3(vertex, 1.0), 1.0);

	vertexOutput.textureCoordinate = imageWorld * vec3(imageCoordinate, 1.0);
	vertexOutput.textureCoordinate.z = float(layerIndex);
	vertexOutput.color = color;
}