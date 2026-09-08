// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Lightmap.hlsl"

uint2 interpolateLightmapCoordinate(Instance instance, uint triangleIndex, float u, float v, float w, uint2 atlasSize) {
	uint indexOffset = (instance.geometryTriangleOffset + triangleIndex) * 3;

	uint indexA = instance.lightmapCoordinateOffset + indexBuffer[indexOffset + 0];
	uint indexB = instance.lightmapCoordinateOffset + indexBuffer[indexOffset + 1];
	uint indexC = instance.lightmapCoordinateOffset + indexBuffer[indexOffset + 2];

	float2 a = lightmapCoordinateBuffer[indexA];
	float2 b = lightmapCoordinateBuffer[indexB];
	float2 c = lightmapCoordinateBuffer[indexC];

	float2 coordinate = a * u + b * v + c * w;

	return coordinate * atlasSize;
}

[shader("closesthit")]
void main(inout Hit hit, BuiltInTriangleIntersectionAttributes attributes) {
	const Arguments arguments = argumentBuffer[0];

	Instance instance = instanceBuffer[InstanceIndex()];

	uint triangleIndex = PrimitiveIndex();

	float v = attributes.barycentrics.x;
	float w = attributes.barycentrics.y;
	float u = 1.0 - v - w;

	uint2 coordinate = interpolateLightmapCoordinate(instance, triangleIndex, u, v, w, arguments.atlasSize);

	float4 gBufferTriangleNormal = gBufferTriangleNormalImage[coordinate];
	float4 gBufferVertexNormal = gBufferVertexNormalImage[coordinate];

	hit.t = RayTCurrent();
	hit.position = WorldRayOrigin() + WorldRayDirection() * hit.t;
	hit.triangleNormal = gBufferTriangleNormal.rgb;
	hit.materialIndex = gBufferVertexNormal.a;
	hit.vertexNormal = gBufferVertexNormal.rgb;
}