#version 460

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#extension GL_EXT_ray_tracing : enable

layout(location = 0) rayPayloadEXT vec3 hitValue;

hitAttributeEXT vec2 attributes;

void main() {
	const vec3 barycentricCoordinates = vec3(1.0 - attributes.x - attributes.y, attributes.x, attributes.y);

	hitValue = barycentricCoordinates;
}