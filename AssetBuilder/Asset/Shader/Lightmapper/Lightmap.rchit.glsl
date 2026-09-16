#version 460

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Lightmap.glsl"

layout(location = 0) rayPayloadInEXT Hit hit;

hitAttributeEXT vec2 attributes;

ivec2 interpolateLightmapCoordinate(Instance instance, uint triangleIndex, float u, float v, float w, uvec2 atlasSize) {
	uint indexOffset = (instance.geometryTriangleOffset + triangleIndex) * 3u;

	uint indexA = instance.lightmapCoordinateOffset + indexBuffer[indexOffset + 0u];
	uint indexB = instance.lightmapCoordinateOffset + indexBuffer[indexOffset + 1u];
	uint indexC = instance.lightmapCoordinateOffset + indexBuffer[indexOffset + 2u];

	vec2 a = lightmapCoordinateBuffer[indexA];
	vec2 b = lightmapCoordinateBuffer[indexB];
	vec2 c = lightmapCoordinateBuffer[indexC];

	vec2 coordinate = a * u + b * v + c * w;

	return ivec2(coordinate * atlasSize);
}

void main() {
	Instance instance = instanceBuffer[gl_InstanceID];

	uint triangleIndex = gl_PrimitiveID;

	float v = attributes.x;
	float w = attributes.y;
	float u = 1.0 - v - w;

	ivec2 coordinate = interpolateLightmapCoordinate(instance, triangleIndex, u, v, w, arguments.atlasSize);

	vec4 gBufferPosition = imageLoad(gBufferPositionImage, coordinate);
	vec4 gBufferTriangleNormal = imageLoad(gBufferTriangleNormalImage, coordinate);
	vec4 gBufferVertexNormal = imageLoad(gBufferVertexNormalImage, coordinate);

	hit.t = gl_HitTEXT;
	hit.position = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * hit.t;
	hit.triangleNormal = gBufferTriangleNormal.rgb;
	hit.triangleIndex = uint(gBufferPosition.a - 1);
	hit.vertexNormal = gBufferVertexNormal.rgb;
	hit.materialIndex = uint(gBufferVertexNormal.a);
}