#version 450

layout(location = 0) in vec2 vertex;
layout(location = 1) in mat3x3 world;

// TODO: Remove unused attribute.
layout(location = 4) in vec4 fillColor;

layout(location = 5) in vec4 z;
layout(location = 6) in uint selected;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
	float zoomLevel;
} camera;

out VertexOutput {
	layout(location = 0) flat uint selected;
} vertexOutput;

void main() {
	vec3 position = camera.projection * world * vec3(vertex, 1.0) * camera.zoomLevel;

	gl_Position = vec4(position.xy, z[0], 1.0);

	vertexOutput.selected = selected;
}