#version 460

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Lightmap.glsl"

hitAttributeEXT vec2 attributes;

layout(location = 0) rayPayloadInEXT Hit hit;

void main() {
	float u = 1.0 - attributes.x - attributes.y;
	float v = attributes.x;
	float w = attributes.y;

	// TODO: triangle offset?
	Triangle triangle_ = loadTriangle(gl_PrimitiveID);

	ivec2 lightmapCoordinate = ivec2((
		triangle_.vertices[0].lightmapCoordinate * u +
		triangle_.vertices[1].lightmapCoordinate * v +
		triangle_.vertices[2].lightmapCoordinate * w
	) * arguments.atlasSize);

	vec4 triangleNormalData = imageLoad(gBufferTriangleNormalImage, lightmapCoordinate);
	vec4 vertexNormalData = imageLoad(gBufferVertexNormalImage, lightmapCoordinate);

	hit.distance = gl_HitTEXT;
	hit.position = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * hit.distance;
	hit.triangleNormal = triangleNormalData.rgb;
	hit.materialIndex = uint(vertexNormalData.a);
	hit.vertexNormal = vertexNormalData.rgb;
}