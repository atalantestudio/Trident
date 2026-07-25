#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec2 lightmapCoordinate;

out Interstage {
	layout(location = 0) flat uint triangleIndex;
	layout(location = 1) vec2 coordinate;
} interstage;

void main() {
	vec2 coordinate = lightmapCoordinate;
	coordinate.y = 1.0 - coordinate.y;

	gl_Position = vec4(coordinate * 2.0 - 1.0, 0.0, 1.0);

	interstage.triangleIndex = uint(gl_VertexIndex / 3);
	interstage.coordinate = lightmapCoordinate;
}