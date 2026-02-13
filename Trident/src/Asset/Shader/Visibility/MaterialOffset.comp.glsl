#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(set = 0, binding = 0) buffer MaterialCountBuffer {
	uint materialCountBuffer[];
};

layout(set = 0, binding = 1) buffer writeonly MaterialOffsetBuffer {
	uint materialOffsetBuffer[];
};

layout(set = 0, binding = 2) buffer writeonly MaterialIndirectBuffer {
	uint materialIndirectBuffer[];
};

void main() {
	uint materialIndex = 0;
	uint sum = 0;

	// TODO: Get current material count.
	const uint materialCount = 2;

	while (materialIndex < materialCount) {
		uint count = materialCountBuffer[materialIndex];

		// Reset material count.
		materialCountBuffer[materialIndex] = 0;

		// TODO: Rewrite as a parallel prefix sum.
		materialOffsetBuffer[materialIndex] = sum;

		materialIndirectBuffer[materialIndex * 3 + 0] = uint(ceil(float(count) / 256.0));
		materialIndirectBuffer[materialIndex * 3 + 1] = 1;
		materialIndirectBuffer[materialIndex * 3 + 2] = 1;

		sum += count;

		materialIndex += 1;
	}
}