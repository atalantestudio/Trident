// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
	float nearPlane;
	float farPlane;
	uint2 viewport;
	float3 position;
};

struct Cluster {
	uint instanceIndex;
};

struct Instance {
	float4x4 world;
	float3x4 normal;
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
	float3 position;
	float3 normal;
	float2 textureCoordinate;
};

struct Primitive {
	Vertex vertices[3];
};

struct BarycentricDerivatives {
	float3 lambda;
	float3 ddx;
	float3 ddy;
};

struct Gradients {
	// Interpolated value.
	float2 value;

	float2 ddx;
	float2 ddy;
};

struct Interpolators {
	uint2 screenCoordinate;
	Instance instance;
	Geometry geometry;
	Primitive primitive;
	BarycentricDerivatives derivatives;
	float3 surface;
	float3 normal;
	Gradients gradients;
};

static const uint CLUSTER_TRIANGLE_COUNT = 128;
static const uint CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3;
static const uint TRIANGLE_INDEX_BIT_COUNT = 7;
static const uint TRIANGLE_INDEX_BIT_MASK = CLUSTER_TRIANGLE_COUNT - 1;

cbuffer Camera : register(b0) {
	Camera camera;
}

StructuredBuffer<uint> materialCoordinateBuffer : register(t0);
Texture2D<uint> visibilityImage : register(t1);
StructuredBuffer<float> vertexPositionComponentBuffer : register(t2);
StructuredBuffer<float> vertexNormalComponentBuffer : register(t3);
StructuredBuffer<float> vertexTextureCoordinateComponentBuffer : register(t4);
StructuredBuffer<uint> vertexPositionIndexBuffer : register(t5);
StructuredBuffer<uint> vertexNormalIndexBuffer : register(t6);
StructuredBuffer<uint> vertexTextureCoordinateIndexBuffer : register(t7);
StructuredBuffer<Cluster> clusterBuffer : register(t8);
StructuredBuffer<Instance> instanceBuffer : register(t9);
StructuredBuffer<Geometry> geometryBuffer : register(t10);
RWTexture2D<float4> outputImage : register(u0);
StructuredBuffer<uint> materialOffsetBuffer : register(t11);

SamplerState armSamplerState : register(s0);
Texture2DArray<float4> armMap : register(t12);
SamplerState diffuseSamplerState : register(s1);
Texture2DArray<float4> diffuseMap : register(t13);
SamplerState normalSamplerState : register(s2);
Texture2DArray<float4> normalMap : register(t14);

float2 ndc(uint2 coordinate) {
	float2 ndc = float2(coordinate) / float2(camera.viewport) * 2.0 - 1.0;
	ndc.y *= -1.0;

	return ndc;
}

float3 loadVertexPosition(Geometry geometry, uint indexIndex) {
	uint index = vertexPositionIndexBuffer[indexIndex];
	uint offset = (geometry.vertexOffset + index) * 3u;

	return float3(
		vertexPositionComponentBuffer[offset + 0],
		vertexPositionComponentBuffer[offset + 1],
		vertexPositionComponentBuffer[offset + 2]
	);
}

float3 loadVertexNormal(Geometry geometry, uint indexIndex) {
	uint index = vertexNormalIndexBuffer[indexIndex];
	uint offset = (geometry.vertexOffset + index) * 3u;

	return float3(
		vertexNormalComponentBuffer[offset + 0],
		vertexNormalComponentBuffer[offset + 1],
		vertexNormalComponentBuffer[offset + 2]
	);
}

float2 loadVertexTextureCoordinate(Geometry geometry, uint indexIndex) {
	uint index = vertexTextureCoordinateIndexBuffer[indexIndex];
	uint offset = (geometry.vertexTextureCoordinateOffset + index) * 2u;

	return float2(
		vertexTextureCoordinateComponentBuffer[offset + 0],
		vertexTextureCoordinateComponentBuffer[offset + 1]
	);
}

Primitive loadPrimitive(Instance instance, Geometry geometry, uint triangleIndex) {
	uint indexOffset = instance.clusterOffset * CLUSTER_INDEX_COUNT + triangleIndex * 3;

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
BarycentricDerivatives computePartialDerivatives(float2 a, float2 b, float2 c) {
	float inverseDeterminant = 1.0 / determinant(float2x2(c - b, a - b));

	BarycentricDerivatives derivatives;
	derivatives.ddx = float3(b.y - c.y, c.y - a.y, a.y - b.y) * inverseDeterminant;
	derivatives.ddy = float3(c.x - b.x, a.x - c.x, b.x - a.x) * inverseDeterminant;

	return derivatives;
}

// `p` is the fragment coordinate in NDC space.
BarycentricDerivatives computeBarycentricDerivatives(float4 a, float4 b, float4 c, float2 p) {
	float3 wInverse = float3(1.0, 1.0, 1.0) / float3(a.w, b.w, c.w);

	float2 pA = a.xy * wInverse.x;
	float2 pB = b.xy * wInverse.y;
	float2 pC = c.xy * wInverse.z;

	BarycentricDerivatives derivatives = computePartialDerivatives(pA, pB, pC);

	derivatives.ddx *= wInverse;
	derivatives.ddy *= wInverse;

	float ddxSum = derivatives.ddx.x + derivatives.ddx.y + derivatives.ddx.z;
	float ddySum = derivatives.ddy.x + derivatives.ddy.y + derivatives.ddy.z;

	float2 delta = p - pA;
	float interpInvW = wInverse.x + delta.x * ddxSum + delta.y * ddySum;
	float interpW = 1.0 / interpInvW;

	derivatives.lambda.x = interpW * (delta.x * derivatives.ddx.x + delta.y * derivatives.ddy.x + wInverse.x);
	derivatives.lambda.y = interpW * (delta.x * derivatives.ddx.y + delta.y * derivatives.ddy.y);
	derivatives.lambda.z = interpW * (delta.x * derivatives.ddx.z + delta.y * derivatives.ddy.z);

	float2 viewport2 = 2.0 / float2(camera.viewport);

	derivatives.ddx *= viewport2.x;
	derivatives.ddy *= -viewport2.y;
	ddxSum *= viewport2.x;
	ddySum *= -viewport2.y;

	derivatives.ddx = (derivatives.lambda * interpInvW + derivatives.ddx) / (interpInvW + ddxSum) - derivatives.lambda;
	derivatives.ddy = (derivatives.lambda * interpInvW + derivatives.ddy) / (interpInvW + ddySum) - derivatives.lambda;

	return derivatives;
}

float interpolate3x1(BarycentricDerivatives derivatives, float a, float b, float c) {
	return dot(derivatives.lambda, float3(a, b, c));
}

Gradients interpolate3x2(BarycentricDerivatives derivatives, float2 a, float2 b, float2 c) {
	float3 x = float3(a.x, b.x, c.x);
	float3 y = float3(a.y, b.y, c.y);

	Gradients gradients;
	gradients.value.x = interpolate3x1(derivatives, x.x, x.y, x.z);
	gradients.value.y = interpolate3x1(derivatives, y.x, y.y, y.z);
	gradients.ddx.x = dot(x, derivatives.ddx);
	gradients.ddx.y = dot(y, derivatives.ddx);
	gradients.ddy.x = dot(x, derivatives.ddy);
	gradients.ddy.y = dot(y, derivatives.ddy);

	return gradients;
}

float3 interpolate3x3(BarycentricDerivatives derivatives, float3 a, float3 b, float3 c) {
	return derivatives.lambda.x * a + derivatives.lambda.y * b + derivatives.lambda.z * c;
}

Interpolators loadInterpolators(uint screenCoordinateIndex) {
	Interpolators interpolators;

	uint screenCoordinate1d = materialOffsetBuffer[0] + screenCoordinateIndex;
	uint packedScreenCoordinate = materialCoordinateBuffer[screenCoordinate1d];

	interpolators.screenCoordinate = uint2(packedScreenCoordinate >> 16, packedScreenCoordinate & 0xffffu);

	uint visibility = visibilityImage[interpolators.screenCoordinate];
	uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1u;

	Cluster cluster = clusterBuffer[clusterIndex];
	interpolators.instance = instanceBuffer[cluster.instanceIndex];
	interpolators.geometry = geometryBuffer[interpolators.instance.geometryIndex];

	uint clusterTriangleIndex = visibility & TRIANGLE_INDEX_BIT_MASK;
	uint triangleIndex = clusterIndex * CLUSTER_TRIANGLE_COUNT + clusterTriangleIndex;

	interpolators.primitive = loadPrimitive(interpolators.instance, interpolators.geometry, triangleIndex);

	float4x4 projectionViewWorld = mul(camera.projectionView, interpolators.instance.world);
	float4 a = mul(projectionViewWorld, float4(interpolators.primitive.vertices[0].position, 1.0));
	float4 b = mul(projectionViewWorld, float4(interpolators.primitive.vertices[1].position, 1.0));
	float4 c = mul(projectionViewWorld, float4(interpolators.primitive.vertices[2].position, 1.0));

	interpolators.derivatives = computeBarycentricDerivatives(a, b, c, ndc(interpolators.screenCoordinate));
	interpolators.surface = mul(interpolators.instance.world, float4(interpolate3x3(interpolators.derivatives, interpolators.primitive.vertices[0].position, interpolators.primitive.vertices[1].position, interpolators.primitive.vertices[2].position), 1.0)).xyz;
	interpolators.normal = normalize(mul((float3x3) interpolators.instance.normal, interpolate3x3(interpolators.derivatives, interpolators.primitive.vertices[0].normal, interpolators.primitive.vertices[1].normal, interpolators.primitive.vertices[2].normal)));
	interpolators.gradients = interpolate3x2(interpolators.derivatives, interpolators.primitive.vertices[0].textureCoordinate, interpolators.primitive.vertices[1].textureCoordinate, interpolators.primitive.vertices[2].textureCoordinate);

	return interpolators;
}

[numthreads(256, 1, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	if (globalInvocationIndex.x >= materialOffsetBuffer[1]) {
		return;
	}

	Interpolators interpolators = loadInterpolators(globalInvocationIndex.x);

	float4 diffuse = diffuseMap.SampleGrad(diffuseSamplerState, float3(interpolators.gradients.value.x, 1.0 - interpolators.gradients.value.y, interpolators.instance.materialIndex - 1u), interpolators.gradients.ddx, interpolators.gradients.ddy);

	outputImage[interpolators.screenCoordinate] = diffuse;
}