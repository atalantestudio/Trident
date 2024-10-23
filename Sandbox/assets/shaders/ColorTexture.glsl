#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;

uniform mat4 u_viewProjection;
uniform mat4 u_transform;

out vec2 v_uv;

void main() {
	gl_Position = vec4(a_position, 1.0f) * u_transform * u_viewProjection;

	v_uv = a_uv;
}

#type fragment
#version 330 core

in vec2 v_uv;

uniform vec4 u_color;
uniform sampler2D u_texture;

layout(location = 0) out vec4 FragColor;

void main() {
	FragColor = texture(u_texture, v_uv) * u_color;
}