struct View {
	uint2 viewport;
};

static const uint TRIANGLE_INDEX_BIT_COUNT = 7;

cbuffer View : register(b0) {
	View view;
}

Texture2D<uint> visibilityImage : register(t0);
ByteAddressBuffer clusterBuffer : register(t1);
ByteAddressBuffer instanceBuffer : register(t2);
RWByteAddressBuffer materialOffsetBuffer : register(u0);
RWTexture2D<uint> materialCoordinateImage : register(u1);

uint fetchInstanceIndex(uint clusterIndex) {
	return clusterBuffer.Load(clusterIndex * 4);
}

uint fetchMaterialIndex(uint instanceIndex) {
	return instanceBuffer.Load(instanceIndex * 80 + 68);
}

[numthreads(16, 16, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 position = globalInvocationIndex.xy;

	if (any(position >= view.viewport)) {
		return;
	}

	uint visibility = visibilityImage[position];
	uint materialIndex = 0;

	if (visibility > 0) {
		uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1;
		uint instanceIndex = fetchInstanceIndex(clusterIndex);

		materialIndex = fetchMaterialIndex(instanceIndex);
	}

	uint materialOffset = InterlockedAdd(materialOffsetBuffer.Load(materialIndex * 4), 1);

	materialCoordinateImage[materialOffset] = position;
}