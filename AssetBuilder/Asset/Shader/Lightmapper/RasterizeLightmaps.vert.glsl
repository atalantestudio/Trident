#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in uint lightmapCoordinateIndex;

layout(set = 0, binding = 0) buffer readonly LightmapCoordinateBuffer {
	vec2 lightmapCoordinates[];
};

out Interstage {
	layout(location = 0) flat uint triangleIndex;
	layout(location = 1) vec2 coordinate;
} interstage;

void main() {
	vec2 coordinate = lightmapCoordinates[lightmapCoordinateIndex];

	vec2 vertex = coordinate;
	vertex.y = 1.0 - vertex.y;
	vertex = vertex * 2.0 - 1.0;

	gl_Position = vec4(vertex, 0.0, 1.0);

	interstage.triangleIndex = uint(gl_VertexIndex / 3);
	interstage.coordinate = coordinate;
}