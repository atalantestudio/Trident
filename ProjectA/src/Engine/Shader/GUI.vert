#version 310 es

layout(location = 0) in vec3 a_position;
layout(location = 1) in mat3x3 a_transform;

/*layout(set = 0, binding = 0) uniform Camera {
	mat3 projection;
} camera;*/

const mat3x3 u_projection = mat3x3(
	2.0 / 1280.0, 0.0, 0.0,
	0.0, -2.0 / 720.0, 0.0,
	-1.0, 1.0, 1.0
);

void main() {
	gl_Position = vec4(u_projection * a_transform * a_position, 1.0);
}