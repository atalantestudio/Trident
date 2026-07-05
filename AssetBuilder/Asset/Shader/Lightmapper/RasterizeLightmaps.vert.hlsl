// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	uint lightmapCoordinateIndex : VERTEX_INPUT_ATTRIBUTE0;
};

struct Interstage {
	float4 position : SV_Position;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 coordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

StructuredBuffer<float2> lightmapCoordinates : register(t0);

Interstage main(VertexInput input) {
	float2 coordinate = lightmapCoordinates[input.lightmapCoordinateIndex];

	float2 vertex = coordinate;
	vertex.y = 1.0 - vertex.y;
	vertex = vertex * 2.0 - 1.0;

	Interstage interstage;
	interstage.position = float4(vertex, 0.0, 1.0);
	interstage.triangleIndex = input.vertexIndex / 3;
	interstage.coordinate = coordinate;

	return interstage;
}