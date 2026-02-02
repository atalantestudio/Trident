RWByteAddressBuffer materialCountBuffer : register(u0);
RWByteAddressBuffer materialOffsetBuffer : register(u1);
RWByteAddressBuffer materialIndirectBuffer : register(u2);

[numthreads(1, 1, 1)]
void main() {
	uint materialIndex = 0;
	uint materialCount;

	materialCountBuffer.GetDimensions(materialCount);

	while (materialIndex < materialCount) {
		// TODO: Rewrite as a parallel prefix sum.
		materialOffsetBuffer.Store(materialIndex * 4, materialCount);

		materialIndirectBuffer.Store(materialIndex * 12 + 0, materialCount / 256);
		materialIndirectBuffer.Store(materialIndex * 12 + 4, 1);
		materialIndirectBuffer.Store(materialIndex * 12 + 8, 1);

		materialCount += materialCountBuffer.Load(materialIndex * 4);

		// Reset material count.
		materialCountBuffer.Store(materialIndex * 4, 0);

		materialIndex += 1;
	}
}