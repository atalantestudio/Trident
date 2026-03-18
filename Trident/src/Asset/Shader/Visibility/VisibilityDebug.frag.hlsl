// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexOutput {
	float4 position : SV_Position;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE1;
};

struct Camera {
	float4x4 projectionView;
	float nearPlane;
	float farPlane;
	uint2 viewport;
	float3 position;
};

struct Vertex {
	float3 position;
	float3 normal;
	float2 textureCoordinate;
};

struct Triangle {
	Vertex vertices[3];
};

struct Cluster {
	uint instanceIndex;
};

struct Instance {
	float4x4 transform;
	uint geometryIndex;
	uint materialIndex;
	uint materialReferenceIndex;
	int clusterOffset;
};

struct Geometry {
	uint vertexOffset;
	uint clusterCount;
};

struct BarycentricDerivatives {
	float3 lambda;
	float3 ddx;
	float3 ddy;
};

struct Gradients {
	float2 value;
	float2 ddx;
	float2 ddy;
};

struct Material {
	float3 ambient;
	float3 diffuse;
	float3 specular;
	float shininess;
};

struct Light {
	float3 position;
	float3 ambient;
	float3 diffuse;
	float3 specular;
};

static const uint CLUSTER_TRIANGLE_COUNT = 128;
static const uint CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3;
static const uint TRIANGLE_INDEX_BIT_COUNT = 7;
static const uint TRIANGLE_INDEX_BIT_MASK = CLUSTER_TRIANGLE_COUNT - 1;
static const float MATERIAL_SPECULAR_MULTIPLIER = 4.0;

// Visibility visualization modes.
static const uint VISUALIZATION_MODE_TRIANGLE_INDEX = 0;
static const uint VISUALIZATION_MODE_CLUSTER_INDEX = 1;
static const uint VISUALIZATION_MODE_LINEAR_DEPTH = 2;

// Scene visualization modes.
static const uint VISUALIZATION_MODE_INSTANCE_INDEX = 3;
static const uint VISUALIZATION_MODE_GEOMETRY_INDEX = 4;
static const uint VISUALIZATION_MODE_MATERIAL_INDEX = 5;

// Primitive visualization modes.
static const uint VISUALIZATION_MODE_TRIANGLE_NORMAL = 6;
static const uint VISUALIZATION_MODE_BARYCENTRIC_DERIVATIVES = 7;
static const uint VISUALIZATION_MODE_VERTEX_NORMAL = 8;
static const uint VISUALIZATION_MODE_VERTEX_TEXTURE_COORDINATE = 9;

// Shading visualization modes.
static const uint VISUALIZATION_MODE_FLAT_SHADING = 10;
static const uint VISUALIZATION_MODE_BLINN_PHONG_SHADING = 11;

// TODO: Use specialization constant.
static const uint VISUALIZATION_MODE = VISUALIZATION_MODE_MATERIAL_INDEX;

cbuffer CameraBuffer : register(b0) {
	Camera camera;
}

Texture2D<uint> visibilityImage : register(t0);
Texture2D<float> depthStencilImage : register(t1);

ByteAddressBuffer vertexPositionComponentBuffer : register(t2);
ByteAddressBuffer vertexNormalComponentBuffer : register(t3);
ByteAddressBuffer vertexTextureCoordinateComponentBuffer : register(t4);
ByteAddressBuffer vertexPositionIndexBuffer : register(t5);
ByteAddressBuffer vertexNormalIndexBuffer : register(t6);
ByteAddressBuffer vertexTextureCoordinateIndexBuffer : register(t7);

ByteAddressBuffer clusterBuffer : register(t10);

ByteAddressBuffer instanceBuffer : register(t11);
ByteAddressBuffer geometryBuffer : register(t12);

Cluster fetchCluster(uint clusterIndex) {
	static const uint stride = 4;

	Cluster cluster;
	cluster.instanceIndex = clusterBuffer.Load(clusterIndex * stride);

	return cluster;
}

Instance fetchInstance(uint instanceIndex) {
	static const uint stride = 80;

	Instance instance;
	instance.transform = transpose(float4x4(
		asfloat(instanceBuffer.Load4(instanceIndex * stride + 0)),
		asfloat(instanceBuffer.Load4(instanceIndex * stride + 16)),
		asfloat(instanceBuffer.Load4(instanceIndex * stride + 32)),
		asfloat(instanceBuffer.Load4(instanceIndex * stride + 48))
	));
	instance.geometryIndex = instanceBuffer.Load(instanceIndex * stride + 64);
	instance.materialIndex = instanceBuffer.Load(instanceIndex * stride + 68);
	instance.materialReferenceIndex = instanceBuffer.Load(instanceIndex * stride + 72);
	instance.clusterOffset = asint(instanceBuffer.Load(instanceIndex * stride + 76));

	return instance;
}

Geometry fetchGeometry(uint geometryIndex) {
	static const uint stride = 8;

	Geometry geometry;
	geometry.vertexOffset = geometryBuffer.Load(geometryIndex * stride + 0);
	geometry.clusterCount = geometryBuffer.Load(geometryIndex * stride + 4);

	return geometry;
}

Material createMaterial(float3 ambient, float3 diffuse, float3 specular, float shininess) {
	Material material;
	material.ambient = ambient;
	material.diffuse = diffuse;
	material.specular = specular;
	material.shininess = shininess;

	return material;
}

Light createLight(float3 position, float3 ambient, float3 diffuse, float3 specular) {
	Light light;
	light.position = position;
	light.ambient = ambient;
	light.diffuse = diffuse;
	light.specular = specular;

	return light;
}

float3 fetchVertexPosition(uint indexIndex, Geometry geometry) {
	uint index = vertexPositionIndexBuffer.Load(indexIndex * 4);
	uint offset = (geometry.vertexOffset + index) * 3;

	return asfloat(vertexPositionComponentBuffer.Load3(offset * 4));
}

float3 fetchVertexNormal(uint indexIndex, Geometry geometry) {
	uint index = vertexNormalIndexBuffer.Load(indexIndex * 4);
	uint offset = (geometry.vertexOffset + index) * 3;

	return asfloat(vertexNormalComponentBuffer.Load3(offset * 4));
}

float2 fetchVertexTextureCoordinate(uint indexIndex, Geometry geometry) {
	uint index = vertexTextureCoordinateIndexBuffer.Load(indexIndex * 4);
	uint offset = (geometry.vertexOffset + index) * 2;

	return asfloat(vertexTextureCoordinateComponentBuffer.Load2(offset * 4));
}

Triangle fetchTriangle(uint triangleIndex, Instance instance, Geometry geometry) {
	uint indexOffset = instance.clusterOffset * CLUSTER_INDEX_COUNT + triangleIndex * 3;

	Triangle _triangle;

	_triangle.vertices[0].position = fetchVertexPosition(indexOffset + 0, geometry);
	_triangle.vertices[1].position = fetchVertexPosition(indexOffset + 1, geometry);
	_triangle.vertices[2].position = fetchVertexPosition(indexOffset + 2, geometry);

	_triangle.vertices[0].normal = fetchVertexNormal(indexOffset + 0, geometry);
	_triangle.vertices[1].normal = fetchVertexNormal(indexOffset + 1, geometry);
	_triangle.vertices[2].normal = fetchVertexNormal(indexOffset + 2, geometry);

	_triangle.vertices[0].textureCoordinate = fetchVertexTextureCoordinate(indexOffset + 0, geometry);
	_triangle.vertices[1].textureCoordinate = fetchVertexTextureCoordinate(indexOffset + 1, geometry);
	_triangle.vertices[2].textureCoordinate = fetchVertexTextureCoordinate(indexOffset + 2, geometry);

	return _triangle;
}

uint murmurMix(uint hash) {
	uint value = hash;
	value ^= value >> 16;
	value *= 0x85ebca6b;
	value ^= value >> 13;
	value *= 0xc2b2ae35;
	value ^= value >> 16;

	return value;
}

float3 uintToColor(uint value) {
	uint hash = murmurMix(value);

	float3 color = float3(
		float((hash >> 0) & 255),
		float((hash >> 8) & 255),
		float((hash >> 16) & 255)
	);

	return color / 255.0;
}

float2 ndc(float2 coordinate) {
	float2 ndc = coordinate / camera.viewport * 2.0 - 1.0;
	ndc.y *= -1.0;

	return ndc;
}

// TODO: Fix linear depth values fading too quick between the near and far planes.
float linearizeDepth(float depth) {
	float nearPlane = camera.nearPlane;
	float farPlane = camera.farPlane;

	return nearPlane * farPlane / (farPlane + depth * (nearPlane - farPlane));
}

// See https://pastebin.com/7H4S6Yw3.
BarycentricDerivatives computePartialDerivatives(float2 a, float2 b, float2 c) {
	float dInverse = 1.0 / determinant(float2x2(c - b, a - b));

	BarycentricDerivatives derivatives;
	derivatives.ddx = float3(b.y - c.y, c.y - a.y, a.y - b.y) * dInverse;
	derivatives.ddy = float3(c.x - b.x, a.x - c.x, b.x - a.x) * dInverse;

	return derivatives;
}

// `p` is the fragment coordinate in NDC space.
BarycentricDerivatives computeBarycentricDerivatives(Triangle _triangle, Instance instance, float2 p) {
	float4x4 projectionViewTransform = mul(camera.projectionView, instance.transform);
	float4 a = mul(projectionViewTransform, float4(_triangle.vertices[0].position, 1.0));
	float4 b = mul(projectionViewTransform, float4(_triangle.vertices[1].position, 1.0));
	float4 c = mul(projectionViewTransform, float4(_triangle.vertices[2].position, 1.0));

	float3 wInverse = 1.0 / float3(a.w, b.w, c.w);

	float2 pA = a.xy * wInverse.x;
	float2 pB = b.xy * wInverse.y;
	float2 pC = c.xy * wInverse.z;

	BarycentricDerivatives derivatives = computePartialDerivatives(pA, pB, pC);

	derivatives.ddx *= wInverse;
	derivatives.ddy *= wInverse;

	float ddxSum = dot(derivatives.ddx, float3(1.0, 1.0, 1.0));
	float ddySum = dot(derivatives.ddy, float3(1.0, 1.0, 1.0));

	float2 delta = p - pA;
	float interpInvW = wInverse.x + delta.x * ddxSum + delta.y * ddySum;
	float interpW = 1.0 / interpInvW;

	derivatives.lambda.x = interpW * (delta.x * derivatives.ddx.x + delta.y * derivatives.ddy.x + wInverse.x);
	derivatives.lambda.y = interpW * (delta.x * derivatives.ddx.y + delta.y * derivatives.ddy.y);
	derivatives.lambda.z = interpW * (delta.x * derivatives.ddx.z + delta.y * derivatives.ddy.z);

	float2 viewport2 = 2.0 / camera.viewport;

	derivatives.ddx *= viewport2.x;
	derivatives.ddy *= -viewport2.y;
	ddxSum *= viewport2.x;
	ddySum *= -viewport2.y;

	derivatives.ddx = (derivatives.lambda * interpInvW + derivatives.ddx) / (interpInvW + ddxSum) - derivatives.lambda;
	derivatives.ddy = (derivatives.lambda * interpInvW + derivatives.ddy) / (interpInvW + ddySum) - derivatives.lambda;

	return derivatives;
}

float interpolate3x1(BarycentricDerivatives derivatives, float a, float b, float c) {
	return derivatives.lambda.x * a + derivatives.lambda.y * b + derivatives.lambda.z * c;
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

float3x3 inverse(float3x3 a) {
	float d = determinant(a);
	float invD = 1.0 / d;

	float3x3 b;
	b[0].x =  (a[1].y * a[2].z - a[2].y * a[1].z) * invD;
	b[1].x = -(a[0].y * a[2].z - a[0].z * a[2].y) * invD;
	b[2].x =  (a[0].y * a[1].z - a[0].z * a[1].y) * invD;
	b[0].y = -(a[1].x * a[2].z - a[1].z * a[2].x) * invD;
	b[1].y =  (a[0].x * a[2].z - a[0].z * a[2].x) * invD;
	b[2].y = -(a[0].x * a[1].z - a[1].x * a[0].z) * invD;
	b[0].z =  (a[1].x * a[2].y - a[2].x * a[1].y) * invD;
	b[1].z = -(a[0].x * a[2].y - a[2].x * a[0].y) * invD;
	b[2].z =  (a[0].x * a[1].y - a[1].x * a[0].y) * invD;

	return b;
}

float3 visualizeTriangleNormal(Triangle _triangle, Instance instance) {
	float3 a = _triangle.vertices[0].position;
	float3 b = _triangle.vertices[1].position;
	float3 c = _triangle.vertices[2].position;

	// TODO: Retrieve the normal matrix from the instance.
	float4x4 T = instance.transform;
	float3x3 N = transpose(inverse(float3x3(
		T[0].xyz,
		T[1].xyz,
		T[2].xyz
	)));

	return normalize(mul(N, cross(b - a, c - a)));
}

float3 visualizeBarycentricDerivatives(Triangle _triangle, Instance instance, float2 coordinate) {
	BarycentricDerivatives derivatives = computeBarycentricDerivatives(_triangle, instance, coordinate);

	return derivatives.lambda;
}

float3 visualizeVertexNormal(Triangle _triangle, Instance instance, float2 coordinate) {
	BarycentricDerivatives derivatives = computeBarycentricDerivatives(_triangle, instance, coordinate);

	// TODO: Retrieve the normal matrix from the instance.
	float4x4 T = instance.transform;
	float3x3 N = transpose(inverse(float3x3(
		T[0].xyz,
		T[1].xyz,
		T[2].xyz
	)));

	return normalize(mul(N, interpolate3x3(derivatives, _triangle.vertices[0].normal, _triangle.vertices[1].normal, _triangle.vertices[2].normal)));
}

float3 visualizeVertexTextureCoordinate(Triangle _triangle, Instance instance, float2 coordinate) {
	BarycentricDerivatives derivatives = computeBarycentricDerivatives(_triangle, instance, coordinate);
	Gradients gradients = interpolate3x2(derivatives, _triangle.vertices[0].textureCoordinate, _triangle.vertices[1].textureCoordinate, _triangle.vertices[2].textureCoordinate);

	return float3(gradients.value, 0.0);
}

float3 visualizeFlatShading(Triangle _triangle, Instance instance, float2 coordinate) {
	BarycentricDerivatives derivatives = computeBarycentricDerivatives(_triangle, instance, coordinate);

	float3 surface = mul(instance.transform, float4(interpolate3x3(derivatives, _triangle.vertices[0].position, _triangle.vertices[1].position, _triangle.vertices[2].position), 1.0)).xyz;
	float3 surfaceToCameraDirection = normalize(camera.position - surface);
	float3 normal = visualizeTriangleNormal(_triangle, instance);

	float diffuse = max(0.0, dot(surfaceToCameraDirection, normal));

	return float3(diffuse, diffuse, diffuse);
}

float3 visualizeBlinnPhongShading(Triangle _triangle, Instance instance, float2 coordinate) {
	const Light DEFAULT_LIGHT = createLight(
		camera.position,
		float3(0.2, 0.2, 0.2),
		float3(0.5, 0.5, 0.5),
		float3(1.0, 1.0, 1.0)
	);

	const Material DEFAULT_MATERIAL = createMaterial(
		float3(1.0, 1.0, 1.0),
		float3(1.0, 1.0, 1.0),
		float3(0.2, 0.2, 0.2),
		100.0
	);

	BarycentricDerivatives derivatives = computeBarycentricDerivatives(_triangle, instance, coordinate);

	float3 surface = mul(instance.transform, float4(interpolate3x3(derivatives, _triangle.vertices[0].position, _triangle.vertices[1].position, _triangle.vertices[2].position), 1.0)).xyz;
	float3 normal = visualizeVertexNormal(_triangle, instance, coordinate);

	// Light position towards surface.
	float3 lightDirection = normalize(DEFAULT_LIGHT.position - surface);

	// Camera position towards surface.
	float3 cameraDirection = normalize(camera.position - surface);

	float3 halfwayDirection = normalize(lightDirection + cameraDirection);

	float diffuseComponent = max(0.0, dot(normal, lightDirection));
	float specularComponent = pow(max(0.0, dot(normal, halfwayDirection)), DEFAULT_MATERIAL.shininess) * MATERIAL_SPECULAR_MULTIPLIER;

	float3 ambient = DEFAULT_LIGHT.ambient * DEFAULT_MATERIAL.ambient;
	float3 diffuse = DEFAULT_LIGHT.diffuse * DEFAULT_MATERIAL.diffuse * diffuseComponent;
	float3 specular = DEFAULT_LIGHT.specular * DEFAULT_MATERIAL.specular * specularComponent;

	return ambient + diffuse + specular;
}

float4 main(VertexOutput input) : SV_Target {
	uint visibility = visibilityImage[input.position.xy];

	if (visibility == 0) {
		if (VISUALIZATION_MODE == VISUALIZATION_MODE_MATERIAL_INDEX) {
			return float4(uintToColor(1), 1.0);
		}

		if (VISUALIZATION_MODE == VISUALIZATION_MODE_LINEAR_DEPTH) {
			return float4(1.0, 1.0, 1.0, 1.0);
		}

		return float4(0.0, 0.0, 0.0, 1.0);
	}

	// Fragment coordinate in NDC space.
	float2 coordinate = ndc(input.position.xy);

	uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1;

	// Triangle index relative to the parent cluster.
	uint clusterTriangleIndex = visibility & TRIANGLE_INDEX_BIT_MASK;

	// Global triangle index.
	uint triangleIndex = clusterIndex * CLUSTER_TRIANGLE_COUNT + clusterTriangleIndex;

	Cluster cluster = fetchCluster(clusterIndex);
	Instance instance = fetchInstance(cluster.instanceIndex);
	Geometry geometry = fetchGeometry(instance.geometryIndex);
	Triangle _triangle = fetchTriangle(triangleIndex, instance, geometry);

	float3 color;

	if (VISUALIZATION_MODE == VISUALIZATION_MODE_TRIANGLE_INDEX) {
		color = uintToColor(triangleIndex + 1);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_CLUSTER_INDEX) {
		color = uintToColor(clusterIndex + 1);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_LINEAR_DEPTH) {
		float nonLinearDepth = depthStencilImage[input.position.xy];
		float linearDepth = linearizeDepth(nonLinearDepth);

		color = float3(linearDepth, linearDepth, linearDepth);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_INSTANCE_INDEX) {
		color = uintToColor(cluster.instanceIndex + 1);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_GEOMETRY_INDEX) {
		color = uintToColor(instance.geometryIndex + 1);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_MATERIAL_INDEX) {
		color = uintToColor(instance.materialIndex + 1);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_TRIANGLE_NORMAL) {
		color = visualizeTriangleNormal(_triangle, instance);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_BARYCENTRIC_DERIVATIVES) {
		color = visualizeBarycentricDerivatives(_triangle, instance, coordinate);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_VERTEX_NORMAL) {
		color = visualizeVertexNormal(_triangle, instance, coordinate);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_VERTEX_TEXTURE_COORDINATE) {
		color = visualizeVertexTextureCoordinate(_triangle, instance, coordinate);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_FLAT_SHADING) {
		color = visualizeFlatShading(_triangle, instance, coordinate);
	}
	else if (VISUALIZATION_MODE == VISUALIZATION_MODE_BLINN_PHONG_SHADING) {
		color = visualizeBlinnPhongShading(_triangle, instance, coordinate);
	}

	return float4(color, 1.0);
}