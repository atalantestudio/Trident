#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec2 coordinate;

out Interstage {
	layout(location = 0) flat uint triangleIndex;
} interstage;

void main() {
	vec2 lightmapCoordinate = coordinate;
	lightmapCoordinate.y = 1.0 - lightmapCoordinate.y;
	lightmapCoordinate = lightmapCoordinate * 2.0 - 1.0;

	gl_Position = vec4(lightmapCoordinate, 0.0, 1.0);

	interstage.triangleIndex = uint(gl_InstanceIndex);
}