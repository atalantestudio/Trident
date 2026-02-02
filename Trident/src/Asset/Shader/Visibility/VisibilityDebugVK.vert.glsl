#version 310 es

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

const vec2 VERTICES[3] = vec2[](
	vec2(-1.0,  1.0),
	vec2( 3.0,  1.0),
	vec2(-1.0, -3.0)
);

void main() {
	vec2 vertex = VERTICES[gl_VertexIndex];

	gl_Position = vec4(vertex, 0.0, 1.0);
}