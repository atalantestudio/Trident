#version 450

layout(location = 0) in vec3 a_position;

layout(location = 0) out vec3 v_color;

void main() {
	gl_Position = vec4(a_position, 1.0);

	v_color = a_position;
}