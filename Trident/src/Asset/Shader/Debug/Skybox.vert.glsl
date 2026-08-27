#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(set = 0, binding = 0) uniform readonly SkyboxCamera {
	mat4x4 projectionView;
} skyboxCamera;

out Interstage {
	layout(location = 0) vec3 direction;
} interstage;

const vec3 POSITIONS[36] = vec3[](
	// Front
	vec3(-0.5,  0.5,  0.5),
	vec3( 0.5,  0.5,  0.5),
	vec3(-0.5, -0.5,  0.5),
	vec3( 0.5,  0.5,  0.5),
	vec3( 0.5, -0.5,  0.5),
	vec3(-0.5, -0.5,  0.5),

	// Back
	vec3( 0.5,  0.5, -0.5),
	vec3(-0.5,  0.5, -0.5),
	vec3( 0.5, -0.5, -0.5),
	vec3(-0.5,  0.5, -0.5),
	vec3(-0.5, -0.5, -0.5),
	vec3( 0.5, -0.5, -0.5),

	// Left
	vec3(-0.5,  0.5, -0.5),
	vec3(-0.5,  0.5,  0.5),
	vec3(-0.5, -0.5, -0.5),
	vec3(-0.5,  0.5,  0.5),
	vec3(-0.5, -0.5,  0.5),
	vec3(-0.5, -0.5, -0.5),

	// Right
	vec3( 0.5,  0.5,  0.5),
	vec3( 0.5,  0.5, -0.5),
	vec3( 0.5, -0.5,  0.5),
	vec3( 0.5,  0.5, -0.5),
	vec3( 0.5, -0.5, -0.5),
	vec3( 0.5, -0.5,  0.5),

	// Top
	vec3(-0.5,  0.5, -0.5),
	vec3( 0.5,  0.5, -0.5),
	vec3(-0.5,  0.5,  0.5),
	vec3( 0.5,  0.5, -0.5),
	vec3( 0.5,  0.5,  0.5),
	vec3(-0.5,  0.5,  0.5),

	// Bottom
	vec3(-0.5, -0.5,  0.5),
	vec3( 0.5, -0.5,  0.5),
	vec3(-0.5, -0.5, -0.5),
	vec3( 0.5, -0.5,  0.5),
	vec3( 0.5, -0.5, -0.5),
	vec3(-0.5, -0.5, -0.5)
);

void main() {
	vec3 position = POSITIONS[gl_VertexIndex];

	gl_Position = skyboxCamera.projectionView * vec4(position, 1.0);

	interstage.direction = position;
}