#version 460

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#extension GL_EXT_ray_tracing : enable

layout(location = 0) rayPayloadEXT vec3 hitValue;

void main() {
	hitValue = vec3(1.0, 0.2, 0.0);
}