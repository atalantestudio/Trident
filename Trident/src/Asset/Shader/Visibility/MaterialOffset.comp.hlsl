// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

RWByteAddressBuffer materialCountBuffer : register(u0);
RWByteAddressBuffer materialOffsetBuffer : register(u1);
RWByteAddressBuffer materialIndirectBuffer : register(u2);

[numthreads(1, 1, 1)]
void main() {
	uint materialIndex = 0;
	uint materialCount;
	uint sum = 0;

	materialCountBuffer.GetDimensions(materialCount);

	while (materialIndex < materialCount) {
		uint count = materialCountBuffer.Load(materialIndex * 4);

		// TODO: Rewrite as a parallel prefix sum.
		materialOffsetBuffer.Store(materialIndex * 4, sum);

		materialIndirectBuffer.Store((materialIndex * 3 + 0) * 4, uint(ceil(float(count) / 256.0)));
		materialIndirectBuffer.Store((materialIndex * 3 + 1) * 4, 1);
		materialIndirectBuffer.Store((materialIndex * 3 + 2) * 4, 1);

		sum += count;

		// Reset material count.
		materialCountBuffer.Store(materialIndex * 4, 0);

		materialIndex += 1;
	}
}