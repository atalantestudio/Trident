// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint indexOffset : SV_VertexID;
	uint indexOffsetOffset : SV_StartVertexLocation;
	uint instanceIndex : SV_InstanceID;
	uint instanceOffset : SV_StartInstanceLocation;
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
	nointerpolation float triangleArea : VERTEX_OUTPUT_ATTRIBUTE13;
};

struct Vertex {
	float3 position;
	uint _0;
	float3 normal;
	uint _1;
};

struct Triangle {
	uint index;
	uint lightmapCoordinateIndexA;
	uint lightmapCoordinateIndexB;
	uint lightmapCoordinateIndexC;
	uint materialIndex;
};

struct Instance {
	float4x3 transform;
	uint geometryTriangleOffset;
	uint deduplicatedTriangleOffset;
	uint lightmapCoordinateOffset;
};

StructuredBuffer<Vertex> vertexBuffer : register(t0);
StructuredBuffer<float2> lightmapCoordinateBuffer : register(t1);
StructuredBuffer<uint> indexBuffer : register(t2);
StructuredBuffer<Triangle> triangleBuffer : register(t3);
StructuredBuffer<Instance> instanceBuffer : register(t4);

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

float calculateArea(float3 a, float3 b, float3 c) {
	return length(cross(b - a, c - a)) * 0.5;
}

Interstage main(VertexInput input) {
	uint triangleIndex = (input.indexOffsetOffset + input.indexOffset) / 3;

	Instance instance = instanceBuffer[input.instanceOffset + input.instanceIndex];
	Triangle triangle_ = triangleBuffer[instance.deduplicatedTriangleOffset + triangleIndex];

	float3x3 transform = (float3x3) instance.transform;

	Vertex a = vertexBuffer[indexBuffer[triangleIndex * 3 + 0]];
	Vertex b = vertexBuffer[indexBuffer[triangleIndex * 3 + 1]];
	Vertex c = vertexBuffer[indexBuffer[triangleIndex * 3 + 2]];

	uint index = indexBuffer[input.indexOffsetOffset + input.indexOffset];

	Vertex vertex = vertexBuffer[index];
	float2 coordinate = lightmapCoordinateBuffer[instance.lightmapCoordinateOffset + index];

	// TODO(asset-builder/lightmapper): Get the atlas size.
	// TODO(asset-builder/lightmapper): This fixes thin geometry receiving no light but adds leaks in larger geometry (e.g. floors and walls).
	// coordinate += LIGHTMAP_COORDINATE_OFFSETS[input.instanceIndex] * 0.5 / 1024.0;

	coordinate.y = 1.0 - coordinate.y;

	Interstage interstage;
	interstage.coordinate = float4(coordinate * 2.0 - 1.0, 0.0, 1.0);
	interstage.position = mul(float4(vertex.position, 1.0), instance.transform);
	interstage.triangleIndex = instance.deduplicatedTriangleOffset + triangleIndex + 1;
	interstage.triangleNormal = mul(vertex.normal, transform);
	interstage.vertexNormal = interstage.triangleNormal;
	interstage.positionA = mul(float4(a.position, 1.0), instance.transform);
	interstage.positionB = mul(float4(b.position, 1.0), instance.transform);
	interstage.positionC = mul(float4(c.position, 1.0), instance.transform);
	interstage.normalA = mul(transform, a.normal);
	interstage.normalB = mul(transform, b.normal);
	interstage.normalC = mul(transform, c.normal);
	interstage.barycentricCoordinate = float3(0.0, 0.0, 0.0);

	switch ((input.indexOffsetOffset + input.indexOffset) % 3) {
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

	interstage.materialIndex = triangle_.materialIndex;
	interstage.triangleArea = calculateArea(interstage.positionA, interstage.positionB, interstage.positionC);

	return interstage;
}