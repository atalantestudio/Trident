#version 450

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

const vec2 VERTICES[3] = {
	vec2(-1.0,  1.0),
	vec2( 3.0,  1.0),
	vec2(-1.0, -3.0)
};

out VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

void main() {
	const vec2 vertex = VERTICES[gl_VertexID];

	gl_Position = vec4(vertex, 0.0, 1.0);

	vertexOutput.textureCoordinate = vertex * 0.5 + 0.5;
}