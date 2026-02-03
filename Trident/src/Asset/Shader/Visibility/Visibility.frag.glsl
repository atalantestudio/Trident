#version 440

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

in VertexOutput {
	layout(location = 0) flat uint visibility;
	layout(location = 1) flat uint materialIndex;
} vertexOutput;

layout(set = 3, binding = 0) buffer writeonly MaterialCountBuffer {
	uint materialCountBuffer[];
};

layout(location = 0) out uint visibility;

void main() {
	visibility = vertexOutput.visibility;

	atomicAdd(materialCountBuffer[vertexOutput.materialIndex], 1);
}