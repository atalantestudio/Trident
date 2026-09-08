#version 460

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Lightmap.glsl"

layout(location = 0) rayPayloadInEXT Hit hit;

void main() {
	hit.distance = -1.0;
}