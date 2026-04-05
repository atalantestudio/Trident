#version 310 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp image2D;
precision highp uimage2D;
precision highp sampler2DArray;

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct Camera {
	mat4x4 projectionView;
	float nearPlane;
	float farPlane;
	uvec2 viewport;
	vec3 position;
};

struct Cluster {
	uint instanceIndex;
};

struct Instance {
	mat4x4 world;
	mat3x3 normal;
	uint geometryIndex;
	uint materialShaderIndex;
	uint materialIndex;
	int clusterOffset;
};

struct Range {
	uint start;
	uint end;
};

const uint CLUSTER_TRIANGLE_COUNT = 128u;
const uint TRIANGLE_INDEX_BIT_COUNT = 7u;
const uint TRIANGLE_INDEX_BIT_MASK = CLUSTER_TRIANGLE_COUNT - 1u;

layout(binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};
layout(binding = 1) buffer readonly MaterialCoordinateBuffer {
	uint materialCoordinateBuffer[];
};
layout(binding = 2, rgba8) uniform writeonly image2D outputImage;
layout(binding = 3, r32ui) uniform readonly uimage2D visibilityImage;
layout(binding = 4) buffer readonly ClusterBuffer {
	Cluster clusterBuffer[];
};
layout(binding = 5) buffer readonly InstanceBuffer {
	Instance instanceBuffer[];
};
layout(binding = 6) buffer readonly MaterialOffsetBuffer {
	Range materialShaderCoordinateIndexRange;
};

layout(binding = 7) uniform sampler2DArray armMap;
layout(binding = 8) uniform sampler2DArray diffuseMap;
layout(binding = 9) uniform sampler2DArray normalMap;

uint murmurMix(uint hash) {
	uint value = hash;
	value ^= value >> 16;
	value *= 0x85ebca6bu;
	value ^= value >> 13;
	value *= 0xc2b2ae35u;
	value ^= value >> 16;

	return value;
}

vec3 uintToColor(uint value) {
	uint hash = murmurMix(value);

	vec3 color = vec3(
		float((hash >> 0) & 255u),
		float((hash >> 8) & 255u),
		float((hash >> 16) & 255u)
	);

	return color / 255.0;
}

void main() {
	if (gl_GlobalInvocationID.x >= materialShaderCoordinateIndexRange.end) {
		return;
	}

	uint pixelIndex = materialShaderCoordinateIndexRange.start + gl_GlobalInvocationID.x;
	uint packedCoordinate = materialCoordinateBuffer[pixelIndex];
	uvec2 screenCoordinate = uvec2(packedCoordinate >> 16, packedCoordinate & 0xffffu);

	uint visibility = imageLoad(visibilityImage, ivec2(screenCoordinate.x, camera.viewport.y - 1u - screenCoordinate.y)).r;
	uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1u;
	Cluster cluster = clusterBuffer[clusterIndex];
	Instance instance = instanceBuffer[cluster.instanceIndex];

	vec3 color = texture(armMap, vec3(0.0, 0.0, instance.materialIndex - 1u)).rgb;

	imageStore(outputImage, ivec2(screenCoordinate), vec4(color, 1.0));
}