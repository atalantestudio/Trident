#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

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
	mat4x4 transform;
	uint geometryIndex;
	uint materialIndex;
	int clusterOffset;
};

const uint TRIANGLE_INDEX_BIT_COUNT = 7;

layout(set = 0, binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};

layout(set = 0, binding = 1, r32ui) uniform readonly uimage2D visibilityImage;

layout(set = 0, binding = 2) buffer readonly ClusterBuffer {
	Cluster clusterBuffer[];
};

layout(set = 0, binding = 3) buffer readonly InstanceBuffer {
	Instance instanceBuffer[];
};

layout(set = 0, binding = 4) buffer writeonly MaterialOffset {
	uint materialOffsetBuffer[];
};

layout(set = 0, binding = 5) buffer writeonly MaterialCoordinate {
	uint materialCoordinateBuffer[];
};

uint fetchInstanceIndex(uint clusterIndex) {
	return clusterBuffer[clusterIndex].instanceIndex;
}

uint fetchMaterialIndex(uint instanceIndex) {
	return instanceBuffer[instanceIndex].materialIndex;
}

void main() {
	uvec2 position = gl_GlobalInvocationID.xy;

	if (position.x >= camera.viewport.x || position.y >= camera.viewport.y) {
		return;
	}

	uint visibility = uint(imageLoad(visibilityImage, ivec2(position)).r);
	uint materialIndex = 0;

	if (visibility > 0) {
		uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1;
		uint instanceIndex = fetchInstanceIndex(clusterIndex);

		// TODO(debug-only): Remove +1.
		materialIndex = fetchMaterialIndex(instanceIndex) + 1;
	}

	uint materialOffset = atomicAdd(materialOffsetBuffer[materialIndex], 1);
	uint packedCoordinate = (gl_GlobalInvocationID.x << 16) | gl_GlobalInvocationID.y;

	materialCoordinateBuffer[materialOffset] = packedCoordinate;
}