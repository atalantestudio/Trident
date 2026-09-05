#version 460

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#extension GL_EXT_ray_tracing : enable

struct Material {
	vec3 emissiveColor;
	float emissiveIntensity;
	vec3 albedo;
};

layout(set = 0, binding = 0) uniform accelerationStructureEXT tlas;

layout(set = 0, binding = 1) buffer readonly ArgumentBuffer {
	uvec2 atlasSize;
	uint triangleCount;
	uint emissiveTriangleCount;
	vec3 skyColor;
	uint _1;
	vec3 sunDirection;
	uint _2;
	vec3 sunColor;
	float sunIntensity;
};

layout(set = 0, binding = 2) buffer readonly MaterialBuffer {
	Material materialBuffer[];
};

layout(set = 0, binding = 3) buffer readonly EmissiveTriangleIndexBuffer {
	uint emissiveTriangleIndexBuffer[];
};

layout(set = 0, binding = 4, rgba32f) uniform readonly image2D gBufferPositionImage;
layout(set = 0, binding = 5, rgba32f) uniform readonly image2D gBufferSmoothPositionImage;
layout(set = 0, binding = 6, rgba32f) uniform readonly image2D gBufferTriangleNormalImage;
layout(set = 0, binding = 7, rgba32f) uniform readonly image2D gBufferVertexNormalImage;

layout(set = 0, binding = 8, rgba8) uniform writeonly image2D intermediateLightmapAtlas;

layout(location = 0) rayPayloadEXT vec3 hitValue;

void main() {
	ivec2 coordinate = ivec2(gl_LaunchIDEXT.xy);

	vec4 gBufferPosition = imageLoad(gBufferPositionImage, coordinate);
	vec4 gBufferSmoothPosition = imageLoad(gBufferSmoothPositionImage, coordinate);
	vec4 gBufferTriangleNormal = imageLoad(gBufferTriangleNormalImage, coordinate);
	vec4 gBufferVertexNormal = imageLoad(gBufferVertexNormalImage, coordinate);

	imageStore(intermediateLightmapAtlas, coordinate, vec4(abs(gBufferTriangleNormal.xyz), 1.0));
}