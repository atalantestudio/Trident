// Copyright 2025 Matteo Legagneux.

#version 450

// Vertex position (already in NDC).
layout(location = 0) in vec2 vertex;

out VertexOutput {
	// Texture coordinate in the range [-1, 1].
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

void main() {
	gl_Position = vec4(vertex, 0.0, 1.0);

	vertexOutput.textureCoordinate = vertex;
}