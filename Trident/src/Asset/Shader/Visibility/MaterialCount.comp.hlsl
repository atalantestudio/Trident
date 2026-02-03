// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
	float nearPlane;
	float farPlane;
	uint2 viewport;
	float3 position;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

RWTexture2D<uint> materialIndexImage : register(u0);
RWByteAddressBuffer materialCountBuffer : register(u1);

[numthreads(8, 8, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 position = globalInvocationIndex.xy;

	if (any(position >= camera.viewport)) {
		return;
	}

	uint materialIndex = materialIndexImage[position];

	InterlockedAdd(materialCountBuffer.Load(materialIndex * 4), 1);
}