// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	uint instanceIndex : SV_InstanceID;
};

struct Interstage {
	float4 coordinate : SV_Position;
	float3 position : VERTEX_OUTPUT_ATTRIBUTE1;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE2;
	nointerpolation float3 triangleNormal : VERTEX_OUTPUT_ATTRIBUTE3;
	float3 vertexNormal : VERTEX_OUTPUT_ATTRIBUTE4;
	nointerpolation float3 positionA : VERTEX_OUTPUT_ATTRIBUTE5;
	nointerpolation float3 positionB : VERTEX_OUTPUT_ATTRIBUTE6;
	nointerpolation float3 positionC : VERTEX_OUTPUT_ATTRIBUTE7;
	nointerpolation float3 normalA : VERTEX_OUTPUT_ATTRIBUTE8;
	nointerpolation float3 normalB : VERTEX_OUTPUT_ATTRIBUTE9;
	nointerpolation float3 normalC : VERTEX_OUTPUT_ATTRIBUTE10;
	float3 barycentricCoordinate : VERTEX_OUTPUT_ATTRIBUTE11;
	nointerpolation uint materialIndex : VERTEX_OUTPUT_ATTRIBUTE12;
};

struct Vertex {
	float3 position;
	float3 normal;
	float4 tangent;
	float2 lightmapCoordinate;
};

StructuredBuffer<Vertex> vertexBuffer : register(t0);
StructuredBuffer<uint> indexBuffer : register(t1);

static const float2 LIGHTMAP_COORDINATE_OFFSETS[25] = {
	float2(-2.0, -2.0),
	float2( 2.0, -2.0),
	float2(-2.0,  2.0),
	float2( 2.0,  2.0),
	float2(-1.0, -2.0),
	float2( 1.0, -2.0),
	float2(-2.0, -1.0),
	float2( 2.0, -1.0),
	float2(-2.0,  1.0),
	float2( 2.0,  1.0),
	float2(-1.0,  2.0),
	float2( 1.0,  2.0),
	float2(-2.0,  0.0),
	float2( 2.0,  0.0),
	float2( 0.0, -2.0),
	float2( 0.0,  2.0),
	float2(-1.0, -1.0),
	float2( 1.0, -1.0),
	float2(-1.0,  0.0),
	float2( 1.0,  0.0),
	float2(-1.0,  1.0),
	float2( 1.0,  1.0),
	float2( 0.0, -1.0),
	float2( 0.0,  1.0),
	float2( 0.0,  0.0),
};

Interstage main(VertexInput input) {
	Vertex a = vertexBuffer[indexBuffer[input.vertexIndex % 3 + 0]];
	Vertex b = vertexBuffer[indexBuffer[input.vertexIndex % 3 + 1]];
	Vertex c = vertexBuffer[indexBuffer[input.vertexIndex % 3 + 2]];
	Vertex vertex = vertexBuffer[indexBuffer[input.vertexIndex]];

	float2 coordinate = vertex.lightmapCoordinate;

	// TODO(asset-builder/lightmapper): Get the atlas size.
	// TODO(asset-builder/lightmapper): This fixes thin geometry receiving no light but adds leaks in larger geometry (e.g. floors and walls).
	// coordinate += LIGHTMAP_COORDINATE_OFFSETS[input.instanceIndex] * 0.5 / 1024.0;

	coordinate.y = 1.0 - coordinate.y;

	Interstage interstage;
	interstage.coordinate = float4(coordinate * 2.0 - 1.0, 1.0, 1.0);
	interstage.position = vertex.position;
	interstage.triangleIndex = input.vertexIndex / 3 + 1;
	interstage.triangleNormal = vertex.normal;
	interstage.vertexNormal = vertex.normal;
	interstage.positionA = a.position;
	interstage.positionB = b.position;
	interstage.positionC = c.position;
	interstage.normalA = a.normal;
	interstage.normalB = b.normal;
	interstage.normalC = c.normal;
	interstage.barycentricCoordinate = float3(0.0, 0.0, 0.0);

	switch (input.vertexIndex % 3) {
		case 0: {
			interstage.barycentricCoordinate[0] = 1.0;

			break;
		}
		case 1: {
			interstage.barycentricCoordinate[1] = 1.0;

			break;
		}
		case 2: {
			interstage.barycentricCoordinate[2] = 1.0;

			break;
		}
	}

	interstage.materialIndex = uint(vertex.tangent.w);

	return interstage;
}