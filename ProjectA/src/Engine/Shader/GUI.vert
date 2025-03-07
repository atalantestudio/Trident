#version 310 es

layout(location = 0) in vec3 a_position;
layout(location = 1) in mat3x3 a_transform;

layout(set = 0, binding = 0) uniform Camera {
	mat3 projection;
} camera;

void main() {
	gl_Position = vec4(camera.projection * a_transform * a_position, 1.0);
}