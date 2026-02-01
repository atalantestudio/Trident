ByteAddressBuffer materialCountBuffer : register(t0);
RWByteAddressBuffer materialOffsetBuffer : register(u1);
// RWByteAddressBuffer materialStoreOffsetBuffer : register(u2);
// RWByteAddressBuffer materialIndirectBuffer : register(u3);

[numthreads(1, 1, 1)]
void main() {
	uint sum = 0;
	uint materialIndex = 0;

	// TODO: Provide material count.
	uint materialCount = 2;

	while (materialIndex < materialCount) {
		uint materialCount = materialCountBuffer.Load(materialIndex * 4);

		materialOffsetBuffer.Store(materialIndex * 4, sum);
		// materialStoreOffsetBuffer.Store(materialIndex * 4, sum);

		// materialIndirectBuffer.Store((materialIndex * 3 + 0) * 4, uint(materialCount / 256.0));
		// materialIndirectBuffer.Store((materialIndex * 3 + 1) * 4, 1);
		// materialIndirectBuffer.Store((materialIndex * 3 + 2) * 4, 1);

		sum += materialCount;
		materialIndex += 1;
	}
}