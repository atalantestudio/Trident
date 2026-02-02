#version 440

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

in VertexOutput {
	layout(location = 0) flat uint visibility;
	layout(location = 1) flat uint materialIndex;
} vertexOutput;

layout(location = 0) out uint visibility;
layout(location = 1) out uint materialIndex;

void main() {
	visibility = vertexOutput.visibility;
	materialIndex = vertexOutput.materialIndex;
}