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

struct Geometry {
	uint vertexOffset;
	uint vertexTextureCoordinateOffset;
	uint clusterCount;
};

struct Range {
	uint start;
	uint end;
};

struct Vertex {
	vec3 position;
	vec3 normal;
	vec2 textureCoordinate;
};

struct Primitive {
	Vertex vertices[3];
};

struct BarycentricDerivatives {
	vec3 lambda;
	vec3 ddx;
	vec3 ddy;
};

struct Gradients {
	// Interpolated value.
	vec2 value;

	vec2 ddx;
	vec2 ddy;
};

struct Interpolators {
	uvec2 screenCoordinate;
	Instance instance;
	Geometry geometry;
	Primitive primitive;
	BarycentricDerivatives derivatives;
	vec3 surface;
	vec3 normal;
	Gradients gradients;
};

const uint CLUSTER_TRIANGLE_COUNT = 128u;
const uint CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3u;
const uint TRIANGLE_INDEX_BIT_COUNT = 7u;
const uint TRIANGLE_INDEX_BIT_MASK = CLUSTER_TRIANGLE_COUNT - 1u;

layout(set = 0, binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};
layout(set = 0, binding = 1) buffer readonly MaterialCoordinateBuffer {
	uint materialCoordinateBuffer[];
};
layout(set = 0, binding = 2, r32ui) uniform readonly uimage2D visibilityImage;
layout(set = 0, binding = 3) buffer readonly VertexPositionComponentBuffer {
	float vertexPositionComponentBuffer[];
};
layout(set = 0, binding = 4) buffer readonly VertexNormalComponentBuffer {
	float vertexNormalComponentBuffer[];
};
layout(set = 0, binding = 5) buffer readonly VertexTextureCoordinateComponentBuffer {
	float vertexTextureCoordinateComponentBuffer[];
};
layout(set = 0, binding = 6) buffer readonly VertexPositionIndexBuffer {
	uint vertexPositionIndexBuffer[];
};
layout(set = 0, binding = 7) buffer readonly VertexNormalIndexBuffer {
	uint vertexNormalIndexBuffer[];
};
layout(set = 0, binding = 8) buffer readonly VertexTextureCoordinateIndexBuffer {
	uint vertexTextureCoordinateIndexBuffer[];
};
layout(set = 0, binding = 9) buffer readonly ClusterBuffer {
	Cluster clusterBuffer[];
};
layout(set = 0, binding = 10) buffer readonly InstanceBuffer {
	Instance instanceBuffer[];
};
layout(set = 0, binding = 11) buffer readonly GeometryBuffer {
	Geometry geometryBuffer[];
};
layout(set = 0, binding = 12, rgba8) uniform writeonly image2D outputImage;
layout(set = 1, binding = 0) buffer readonly MaterialOffsetBuffer {
	Range materialShaderCoordinateIndexRange;
};

layout(set = 1, binding = 1) uniform sampler2DArray armMap;
layout(set = 1, binding = 2) uniform sampler2DArray diffuseMap;
layout(set = 1, binding = 3) uniform sampler2DArray normalMap;

vec2 ndc(uvec2 coordinate) {
	vec2 ndc = vec2(coordinate) / vec2(camera.viewport) * 2.0 - 1.0;
	ndc.y *= -1.0;

	return ndc;
}

vec3 loadVertexPosition(Geometry geometry, uint indexIndex) {
	uint index = vertexPositionIndexBuffer[indexIndex];
	uint offset = (geometry.vertexOffset + index) * 3u;

	float x = vertexPositionComponentBuffer[offset + 0u];
	float y = vertexPositionComponentBuffer[offset + 1u];
	float z = vertexPositionComponentBuffer[offset + 2u];

	return vec3(x, y, z);
}

vec3 loadVertexNormal(Geometry geometry, uint indexIndex) {
	uint index = vertexNormalIndexBuffer[indexIndex];
	uint offset = (geometry.vertexOffset + index) * 3u;

	float x = vertexNormalComponentBuffer[offset + 0u];
	float y = vertexNormalComponentBuffer[offset + 1u];
	float z = vertexNormalComponentBuffer[offset + 2u];

	return vec3(x, y, z);
}

vec2 loadVertexTextureCoordinate(Geometry geometry, uint indexIndex) {
	uint index = vertexTextureCoordinateIndexBuffer[indexIndex];
	uint offset = (geometry.vertexTextureCoordinateOffset + index) * 2u;

	float x = vertexTextureCoordinateComponentBuffer[offset + 0u];
	float y = vertexTextureCoordinateComponentBuffer[offset + 1u];

	return vec2(x, y);
}

Primitive loadPrimitive(Instance instance, Geometry geometry, uint triangleIndex) {
	uint indexOffset = uint(instance.clusterOffset * int(CLUSTER_INDEX_COUNT) + int(triangleIndex) * 3);

	Primitive primitive;
	primitive.vertices[0].position = loadVertexPosition(geometry, indexOffset + 0u);
	primitive.vertices[1].position = loadVertexPosition(geometry, indexOffset + 1u);
	primitive.vertices[2].position = loadVertexPosition(geometry, indexOffset + 2u);
	primitive.vertices[0].normal = loadVertexNormal(geometry, indexOffset + 0u);
	primitive.vertices[1].normal = loadVertexNormal(geometry, indexOffset + 1u);
	primitive.vertices[2].normal = loadVertexNormal(geometry, indexOffset + 2u);
	primitive.vertices[0].textureCoordinate = loadVertexTextureCoordinate(geometry, indexOffset + 0u);
	primitive.vertices[1].textureCoordinate = loadVertexTextureCoordinate(geometry, indexOffset + 1u);
	primitive.vertices[2].textureCoordinate = loadVertexTextureCoordinate(geometry, indexOffset + 2u);

	return primitive;
}

// See https://pastebin.com/7H4S6Yw3.
BarycentricDerivatives computePartialDerivatives(vec2 a, vec2 b, vec2 c) {
	float inverseDeterminant = 1.0 / determinant(mat2x2(c - b, a - b));

	BarycentricDerivatives derivatives;
	derivatives.ddx = vec3(b.y - c.y, c.y - a.y, a.y - b.y) * inverseDeterminant;
	derivatives.ddy = vec3(c.x - b.x, a.x - c.x, b.x - a.x) * inverseDeterminant;

	return derivatives;
}

// `p` is the fragment coordinate in NDC space.
BarycentricDerivatives computeBarycentricDerivatives(vec4 a, vec4 b, vec4 c, vec2 p) {
	vec3 wInverse = vec3(1.0, 1.0, 1.0) / vec3(a.w, b.w, c.w);

	vec2 pA = a.xy * wInverse.x;
	vec2 pB = b.xy * wInverse.y;
	vec2 pC = c.xy * wInverse.z;

	BarycentricDerivatives derivatives = computePartialDerivatives(pA, pB, pC);

	derivatives.ddx *= wInverse;
	derivatives.ddy *= wInverse;

	float ddxSum = derivatives.ddx.x + derivatives.ddx.y + derivatives.ddx.z;
	float ddySum = derivatives.ddy.x + derivatives.ddy.y + derivatives.ddy.z;

	vec2 delta = p - pA;
	float interpInvW = wInverse.x + delta.x * ddxSum + delta.y * ddySum;
	float interpW = 1.0 / interpInvW;

	derivatives.lambda.x = interpW * (delta.x * derivatives.ddx.x + delta.y * derivatives.ddy.x + wInverse.x);
	derivatives.lambda.y = interpW * (delta.x * derivatives.ddx.y + delta.y * derivatives.ddy.y);
	derivatives.lambda.z = interpW * (delta.x * derivatives.ddx.z + delta.y * derivatives.ddy.z);

	vec2 viewport2 = 2.0 / vec2(camera.viewport);

	derivatives.ddx *= viewport2.x;
	derivatives.ddy *= -viewport2.y;
	ddxSum *= viewport2.x;
	ddySum *= -viewport2.y;

	derivatives.ddx = (derivatives.lambda * interpInvW + derivatives.ddx) / (interpInvW + ddxSum) - derivatives.lambda;
	derivatives.ddy = (derivatives.lambda * interpInvW + derivatives.ddy) / (interpInvW + ddySum) - derivatives.lambda;

	return derivatives;
}

float interpolate3x1(BarycentricDerivatives derivatives, float a, float b, float c) {
	return dot(derivatives.lambda, vec3(a, b, c));
}

Gradients interpolate3x2(BarycentricDerivatives derivatives, vec2 a, vec2 b, vec2 c) {
	vec3 x = vec3(a.x, b.x, c.x);
	vec3 y = vec3(a.y, b.y, c.y);

	Gradients gradients;
	gradients.value.x = interpolate3x1(derivatives, x.x, x.y, x.z);
	gradients.value.y = interpolate3x1(derivatives, y.x, y.y, y.z);
	gradients.ddx.x = dot(x, derivatives.ddx);
	gradients.ddx.y = dot(y, derivatives.ddx);
	gradients.ddy.x = dot(x, derivatives.ddy);
	gradients.ddy.y = dot(y, derivatives.ddy);

	return gradients;
}

vec3 interpolate3x3(BarycentricDerivatives derivatives, vec3 a, vec3 b, vec3 c) {
	return derivatives.lambda.x * a + derivatives.lambda.y * b + derivatives.lambda.z * c;
}

Interpolators loadInterpolators(uint screenCoordinateIndex) {
	Interpolators interpolators;

	uint screenCoordinate1d = materialShaderCoordinateIndexRange.start + screenCoordinateIndex;
	uint packedScreenCoordinate = materialCoordinateBuffer[screenCoordinate1d];

	interpolators.screenCoordinate = uvec2(packedScreenCoordinate >> 16, packedScreenCoordinate & 0xffffu);

	uint visibility = imageLoad(visibilityImage, ivec2(interpolators.screenCoordinate)).r;
	uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1u;
	Cluster cluster = clusterBuffer[clusterIndex];

	interpolators.instance = instanceBuffer[cluster.instanceIndex];
	interpolators.geometry = geometryBuffer[interpolators.instance.geometryIndex];

	uint clusterTriangleIndex = visibility & TRIANGLE_INDEX_BIT_MASK;
	uint triangleIndex = clusterIndex * CLUSTER_TRIANGLE_COUNT + clusterTriangleIndex;

	interpolators.primitive = loadPrimitive(interpolators.instance, interpolators.geometry, triangleIndex);

	mat4x4 projectionViewWorld = camera.projectionView * interpolators.instance.world;
	vec4 a = projectionViewWorld * vec4(interpolators.primitive.vertices[0].position, 1.0);
	vec4 b = projectionViewWorld * vec4(interpolators.primitive.vertices[1].position, 1.0);
	vec4 c = projectionViewWorld * vec4(interpolators.primitive.vertices[2].position, 1.0);

	interpolators.derivatives = computeBarycentricDerivatives(a, b, c, ndc(interpolators.screenCoordinate));
	interpolators.surface = (interpolators.instance.world * vec4(interpolate3x3(interpolators.derivatives, interpolators.primitive.vertices[0].position, interpolators.primitive.vertices[1].position, interpolators.primitive.vertices[2].position), 1.0)).xyz;
	interpolators.normal = normalize(interpolators.instance.normal * interpolate3x3(interpolators.derivatives, interpolators.primitive.vertices[0].normal, interpolators.primitive.vertices[1].normal, interpolators.primitive.vertices[2].normal));
	interpolators.gradients = interpolate3x2(interpolators.derivatives, interpolators.primitive.vertices[0].textureCoordinate, interpolators.primitive.vertices[1].textureCoordinate, interpolators.primitive.vertices[2].textureCoordinate);

	return interpolators;
}

void main() {
	if (gl_GlobalInvocationID.x >= materialShaderCoordinateIndexRange.end) {
		return;
	}

	Interpolators interpolators = loadInterpolators(gl_GlobalInvocationID.x);

	vec4 diffuse = textureGrad(diffuseMap, vec3(interpolators.gradients.value.x, 1.0 - interpolators.gradients.value.y, interpolators.instance.materialIndex - 1u), interpolators.gradients.ddx, interpolators.gradients.ddy);

	imageStore(outputImage, ivec2(interpolators.screenCoordinate), diffuse);
}