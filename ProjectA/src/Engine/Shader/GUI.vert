#version 310 es

layout(location = 0) in vec3 a_position;
layout(location = 1) in mat3x3 a_transform;
layout(location = 4) in uint a_colorIndex;
layout(location = 5) in uint a_textureIndex;

layout(location = 0) flat out uint v_colorIndex;
layout(location = 1) flat out uint v_textureIndex;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

void main() {
	gl_Position = vec4(camera.projection * a_transform * a_position, 1.0);

	v_colorIndex = a_colorIndex;
	v_textureIndex = a_textureIndex;
}