// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
	float nearPlane;
	float farPlane;
	uint2 viewport;
	float3 position;
};

static const uint TRIANGLE_INDEX_BIT_COUNT = 7;

cbuffer Camera : register(b0) {
	Camera camera;
}

Texture2D<uint> visibilityImage : register(t0);
ByteAddressBuffer clusterBuffer : register(t1);
ByteAddressBuffer instanceBuffer : register(t2);
RWByteAddressBuffer materialOffsetBuffer : register(u0);
RWByteAddressBuffer materialCoordinateBuffer : register(u1);

uint fetchInstanceIndex(uint clusterIndex) {
	return clusterBuffer.Load(clusterIndex * 4);
}

uint fetchMaterialIndex(uint instanceIndex) {
	return instanceBuffer.Load(instanceIndex * 80 + 68);
}

[numthreads(16, 16, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 position = globalInvocationIndex.xy;

	if (any(position >= camera.viewport)) {
		return;
	}

	uint visibility = visibilityImage[position];
	uint materialIndex = 0;

	if (visibility > 0) {
		uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1;
		uint instanceIndex = fetchInstanceIndex(clusterIndex);

		materialIndex = fetchMaterialIndex(instanceIndex);
	}

	uint materialOffset;

	materialOffsetBuffer.InterlockedAdd(materialIndex * 4, 1, materialOffset);

	uint packedCoordinate = (globalInvocationIndex.x << 16) | globalInvocationIndex.y;

	materialCoordinateBuffer.Store(materialOffset * 4, packedCoordinate);
}