// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 position : SV_Position;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE1;
};

struct FragmentOutput {
	float4 color : SV_Target;
};

uint murmurMix(uint hash) {
	hash ^= hash >> 16;
	hash *= 0x85ebca6bu;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35u;
	hash ^= hash >> 16;

	return hash;
}

float3 uintToColor(uint value) {
	uint hash = murmurMix(value);

	return float3(float(hash & 255), float((hash >> 8) & 255), float((hash >> 16) & 255)) / 255.0;
}

FragmentOutput main(Interstage interstage) {
	FragmentOutput output;
	output.color = float4(uintToColor(interstage.triangleIndex + 1), 1.0);

	return output;
}