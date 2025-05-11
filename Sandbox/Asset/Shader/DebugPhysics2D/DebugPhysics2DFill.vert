#version 450

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in mat3x3 world;
layout(location = 4) in vec4 fillColor;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
	float zoomLevel;
} camera;

out VertexOutput {
	layout(location = 0) vec4 fillColor;
} vertexOutput;

void main() {
	vec3 position = camera.projection * world * vec3(vertexPosition, 1.0) * camera.zoomLevel;

	gl_Position = vec4(position.xy, 0.0, 1.0);

	vertexOutput.fillColor = fillColor;
}