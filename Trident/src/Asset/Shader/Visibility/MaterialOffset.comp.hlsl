// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct D3D12_DISPATCH_ARGUMENTS {
	uint ThreadGroupCountX;
	uint ThreadGroupCountY;
	uint ThreadGroupCountZ;
};

RWStructuredBuffer<uint> materialCountBuffer : register(u0);
RWStructuredBuffer<uint> materialOffsetBuffer : register(u1);
RWStructuredBuffer<D3D12_DISPATCH_ARGUMENTS> materialIndirectBuffer : register(u2);

[numthreads(1, 1, 1)]
void main() {
	uint materialIndex = 0;
	uint materialCount;
	uint sum = 0;
	uint _;

	materialCountBuffer.GetDimensions(materialCount, _);

	while (materialIndex < materialCount) {
		uint count = materialCountBuffer[materialIndex];

		// TODO: Rewrite as a parallel prefix sum.
		materialOffsetBuffer[materialIndex] = sum;

		materialIndirectBuffer[materialIndex].ThreadGroupCountX = uint(ceil(float(count) / 256.0));
		materialIndirectBuffer[materialIndex].ThreadGroupCountY = 1;
		materialIndirectBuffer[materialIndex].ThreadGroupCountZ = 1;

		sum += count;

		// Reset material count.
		materialCountBuffer[materialIndex] = 0;

		materialIndex += 1;
	}
}