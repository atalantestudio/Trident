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
	float3x3 normal;
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
RWStructuredBuffer<uint> materialCountBuffer : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	if (any(globalInvocationIndex.xy >= camera.viewport)) {
		return;
	}

	uint visibility = visibilityImage[globalInvocationIndex.xy];
	uint materialShaderIndex = 0;

	if (visibility > 0) {
		uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1;
		uint instanceIndex = clusterBuffer[clusterIndex].instanceIndex;

		materialShaderIndex = instanceBuffer[instanceIndex].materialShaderIndex;
	}

	InterlockedAdd(materialCountBuffer[materialShaderIndex], 1);
}