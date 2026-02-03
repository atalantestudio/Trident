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

ByteAddressBuffer materialCoordinateBuffer : register(t0);
RWTexture2D<float4> outputImage : register(u0);
Texture2D<uint> visibilityImage : register(t1);

[numthreads(256, 1, 1)]
void main() {
	// TODO
}