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
	float4x4 world;
	float3x4 normal;
	uint geometryIndex;
	uint materialShaderIndex;
	uint materialIndex;
	int clusterOffset;
};

static const uint TRIANGLE_INDEX_BIT_COUNT = 7;

cbuffer Camera : register(b0) {
	Camera camera;
}

Texture2D<uint> visibilityImage : register(t0);
StructuredBuffer<Cluster> clusterBuffer : register(t1);
StructuredBuffer<Instance> instanceBuffer : register(t2);
RWStructuredBuffer<uint> materialOffsetBuffer : register(u0);
RWStructuredBuffer<uint> materialCoordinateBuffer : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 position = globalInvocationIndex.xy;

	if (any(position >= camera.viewport)) {
		return;
	}

	uint visibility = visibilityImage[position];
	uint materialShaderIndex = 0;

	if (visibility > 0) {
		uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1;
		uint instanceIndex = clusterBuffer[clusterIndex].instanceIndex;

		materialShaderIndex = instanceBuffer[instanceIndex].materialShaderIndex;
	}

	uint materialOffset;

	InterlockedAdd(materialOffsetBuffer[materialShaderIndex], 1, materialOffset);

	uint packedCoordinate = (globalInvocationIndex.x << 16) | globalInvocationIndex.y;

	materialCoordinateBuffer[materialOffset] = packedCoordinate;
}