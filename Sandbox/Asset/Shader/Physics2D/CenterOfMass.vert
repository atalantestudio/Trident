#version 450

layout(location = 0) in vec3 vertex;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
	float zoomLevel;
} camera;

void main() {
	vec3 position = camera.projection * vec3(vertex.xy, 1.0) * camera.zoomLevel;

	gl_Position = vec4(position.xy, vertex.z, 1.0);
	gl_PointSize = 0.325 * camera.zoomLevel;
}