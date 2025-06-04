#version 450

layout(location = 0) in vec2 vertex;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
	float zoomLevel;
} camera;

layout(set = 0, binding = 1) uniform Placeholder {
	vec2 offset;
} placeholder;

void main() {
	vec3 position = camera.projection * vec3(vertex + placeholder.offset, 1.0) * camera.zoomLevel;

	gl_Position = vec4(position.xy, 0.0, 1.0);
	gl_PointSize = 0.325 * camera.zoomLevel;
}