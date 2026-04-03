// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
	float nearPlane;
	float farPlane;
	uint2 viewport;
	float3 position;
};

struct Cluster {
	uint instanceIndex;
};

struct Instance {
	float4x4 transform;
	uint geometryIndex;
	uint materialShaderIndex;
	uint materialIndex;
	int clusterOffset;
};

static const uint CLUSTER_TRIANGLE_COUNT = 128;
static const uint TRIANGLE_INDEX_BIT_COUNT = 7;
static const uint TRIANGLE_INDEX_BIT_MASK = CLUSTER_TRIANGLE_COUNT - 1;

cbuffer Camera : register(b0) {
	Camera camera;
}

StructuredBuffer<uint> materialCoordinateBuffer : register(t0);
RWTexture2D<float4> outputImage : register(u0);
Texture2D<uint> visibilityImage : register(t1);
ByteAddressBuffer clusterBuffer : register(t2);
StructuredBuffer<Instance> instanceBuffer : register(t3);
StructuredBuffer<uint> materialOffsetBuffer : register(t4);

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

Cluster loadCluster(uint clusterIndex) {
	Cluster cluster;
	cluster.instanceIndex = clusterBuffer.Load(clusterIndex * 4);

	return cluster;
}

[numthreads(256, 1, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	if (globalInvocationIndex.x >= materialOffsetBuffer[1]) {
		return;
	}

	uint pixelIndex = materialOffsetBuffer[0] + globalInvocationIndex.x;
	uint packedCoordinate = materialCoordinateBuffer[pixelIndex];
	uint2 screenCoordinate = uint2(packedCoordinate >> 16, packedCoordinate & 0xffff);

	uint visibility = visibilityImage[screenCoordinate];
	uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1;
	// uint clusterTriangleIndex = visibility & TRIANGLE_INDEX_BIT_MASK;
	// uint triangleIndex = clusterIndex * CLUSTER_TRIANGLE_COUNT + clusterTriangleIndex;

	Cluster cluster = loadCluster(clusterIndex);
	Instance instance = instanceBuffer[cluster.instanceIndex];

	float3 color = uintToColor(instance.materialIndex + 1);
	// float3 color = uintToColor(triangleIndex + 1);

	outputImage[screenCoordinate] = float4(color, 1.0);
}