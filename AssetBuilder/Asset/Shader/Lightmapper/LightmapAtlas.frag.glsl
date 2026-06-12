#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) flat uint triangleIndex;
} interstage;

layout(location = 0) out vec4 fragment;

uint murmurMix(uint hash) {
	hash ^= hash >> 16;
	hash *= 0x85ebca6bu;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35u;
	hash ^= hash >> 16;

	return hash;
}

vec3 uintToColor(uint value) {
	uint hash = murmurMix(value);

	return vec3(float(hash & 255u), float((hash >> 8) & 255u), float((hash >> 16) & 255u)) / 255.0;
}

void main() {
	fragment = vec4(uintToColor(interstage.triangleIndex), 1.0);
}