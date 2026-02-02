RWByteAddressBuffer materialCountBuffer : register(u0);
RWByteAddressBuffer materialOffsetBuffer : register(u1);
RWByteAddressBuffer materialIndirectBuffer : register(u2);

[numthreads(1, 1, 1)]
void main() {
	uint sum = 0;
	uint materialIndex = 0;
	uint materialCount;

	materialCountBuffer.GetDimensions(materialCount);

	while (materialIndex < materialCount) {
		uint materialCount = materialCountBuffer.Load(materialIndex * 4);

		// Reset material count.
		materialCountBuffer.Store(materialIndex * 4, 0);

		// TODO: Rewrite as a parallel prefix sum.
		materialOffsetBuffer.Store(materialIndex * 4, sum);

		materialIndirectBuffer.Store(materialIndex * 12 + 0, uint(materialCount / 256.0));
		materialIndirectBuffer.Store(materialIndex * 12 + 4, 1);
		materialIndirectBuffer.Store(materialIndex * 12 + 8, 1);

		sum += materialCount;
		materialIndex += 1;
	}
}