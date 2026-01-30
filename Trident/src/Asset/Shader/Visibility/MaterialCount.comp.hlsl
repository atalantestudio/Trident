// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct View {
	uint2 viewport;
};

cbuffer View : register(b0) {
	View view;
}

RWTexture2D<uint> materialIndexImage : register(u0);
RWByteAddressBuffer materialCountBuffer : register(u1);

[numthreads(8, 8, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 position = globalInvocationIndex.xy;

	if (any(position >= view.viewport)) {
		return;
	}

	uint materialIndex = materialIndexImage[position];

	InterlockedAdd(materialCountBuffer.Load(materialIndex * 4), 1);
}