#version 450

layout(location = 0) in vec2 vertex;
layout(location = 1) in mat3x3 world;
layout(location = 4) in vec4 fillColor;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
	float zoomLevel;
} camera;

layout(set = 0, binding = 1) uniform Placeholder {
	vec2 offset;
} placeholder;

out VertexOutput {
	layout(location = 0) vec4 fillColor;
} vertexOutput;

void main() {
	mat3x3 worldOffset = mat3x3(
		world[0][0], world[0][1], world[0][2],
		world[1][0], world[1][1], world[1][2],
		world[2][0] + placeholder.offset.x, world[2][1] + placeholder.offset.y, world[2][2]
	);

	vec3 position = camera.projection * worldOffset * vec3(vertex, 1.0) * camera.zoomLevel;

	gl_Position = vec4(position.xy, 0.0, 1.0);

	vertexOutput.fillColor = vec4(fillColor.rgb, 0.5);
}