// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
	float nearPlane;
	float farPlane;
	uint2 viewport;
	float3 position;
};

static const uint MATERIAL_INDEX = 1;
static const uint CLUSTER_TRIANGLE_COUNT = 128;
static const uint TRIANGLE_INDEX_BIT_COUNT = 7;
static const uint TRIANGLE_INDEX_BIT_MASK = CLUSTER_TRIANGLE_COUNT - 1;

cbuffer Camera : register(b0) {
	Camera camera;
}

ByteAddressBuffer materialCoordinateBuffer : register(t0);
ByteAddressBuffer materialOffsetBuffer : register(t1);
RWTexture2D<float4> outputImage : register(u0);
Texture2D<uint> visibilityImage : register(t2);

uint murmurMix(uint hash) {
	uint value = hash;
	value ^= value >> 16;
	value *= 0x85ebca6b;
	value ^= value >> 13;
	value *= 0xc2b2ae35;
	value ^= value >> 16;

	return value;
}

float3 uintToColor(uint value) {
	uint hash = murmurMix(value);

	float3 color = float3(
		float((hash >> 0) & 255),
		float((hash >> 8) & 255),
		float((hash >> 16) & 255)
	);

	return color / 255.0;
}

[numthreads(256, 1, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	if (globalInvocationIndex.x >= materialOffsetBuffer.Load(MATERIAL_INDEX * 4)) {
		return;
	}

	uint pixelIndex = materialOffsetBuffer.Load((MATERIAL_INDEX - 1) * 4) + globalInvocationIndex.x;
	uint packedCoordinate = materialCoordinateBuffer.Load(pixelIndex * 4);
	uint2 coordinate = uint2(packedCoordinate >> 16, packedCoordinate & 0xffff);

	uint visibility = visibilityImage[coordinate];
	uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1;
	uint clusterTriangleIndex = visibility & TRIANGLE_INDEX_BIT_MASK;
	uint triangleIndex = clusterIndex * CLUSTER_TRIANGLE_COUNT + clusterTriangleIndex;

	float3 color = uintToColor(triangleIndex + 1);

	// outputImage[coordinate] = float4(color, 1.0);
	outputImage[coordinate] = float4(1.0, 0.2, 0.0, 1.0);
}