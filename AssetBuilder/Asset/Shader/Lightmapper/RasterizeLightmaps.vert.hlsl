// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint instanceIndex : SV_InstanceID;
	float2 coordinate : VERTEX_INPUT_ATTRIBUTE0;
};

struct Interstage {
	float4 position : SV_Position;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 coordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

Interstage main(VertexInput input) {
	float2 lightmapCoordinate = input.coordinate;
	lightmapCoordinate.y = 1.0 - lightmapCoordinate.y;
	lightmapCoordinate = lightmapCoordinate * 2.0 - 1.0;

	Interstage interstage;
	interstage.position = float4(lightmapCoordinate, 0.0, 1.0);
	interstage.triangleIndex = input.instanceIndex;
	interstage.coordinate = input.coordinate;

	return interstage;
}