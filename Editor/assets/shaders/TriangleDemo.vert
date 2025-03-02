#version 310 es

const vec2 VERTEX_POSITIONS[3] = vec2[3](
	vec2(-0.5, -0.5),
	vec2(0.5, -0.5),
	vec2(0, 0.5)
);

layout(location = 0) out vec3 v_color;

void main() {
	vec2 position = VERTEX_POSITIONS[gl_VertexIndex];

	gl_Position = vec4(position, 0.0, 1.0);

	v_color = vec3(position, 0.0);
}