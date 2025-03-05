#version 310 es

layout(location = 0) in mat3x3 a_world;

const vec3 VERTEX_POSITIONS[] = vec3[](
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 1.0, 1.0),
	vec3(1.0, 1.0, 1.0)
);

const mat3x3 u_projection = mat3x3(
	2.0 / 1280.0, 0.0, 0.0,
	0.0, -2.0 / 720.0, 0.0,
	-1.0, 1.0, 1.0
);

void main() {
	vec3 position = VERTEX_POSITIONS[gl_VertexID];

	gl_Position = vec4(u_projection * a_world * position, 1.0);
}