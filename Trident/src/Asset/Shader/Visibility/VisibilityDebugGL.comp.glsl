#version 310 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp image2D;
precision highp uimage2D;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

struct Camera {
	mat4x4 projectionView;
	float nearPlane;
	float farPlane;
	uvec2 viewport;
	vec3 position;
};

struct Vertex {
	vec3 position;
	vec3 normal;
	vec2 textureCoordinate;
};

struct Triangle {
	Vertex vertices[3];
};

struct Cluster {
	uint instanceIndex;
};

struct Instance {
	mat4x4 transform;
	uint geometryIndex;
	uint materialShaderIndex;
	uint materialIndex;
	int clusterOffset;
};

struct Geometry {
	uint vertexOffset;
	uint clusterCount;
};

struct BarycentricDerivatives {
	vec3 lambda;
	vec3 ddx;
	vec3 ddy;
};

struct Gradients {
	vec2 value;
	vec2 ddx;
	vec2 ddy;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

const uint CLUSTER_TRIANGLE_COUNT = 128u;
const uint CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3u;
const uint TRIANGLE_INDEX_BIT_COUNT = 7u;
const uint TRIANGLE_INDEX_BIT_MASK = CLUSTER_TRIANGLE_COUNT - 1u;
const float MATERIAL_SPECULAR_MULTIPLIER = 4.0;

// Visibility visualization modes.
const uint VISUALIZATION_MODE_TRIANGLE_INDEX = 0u;
const uint VISUALIZATION_MODE_CLUSTER_INDEX = 1u;
const uint VISUALIZATION_MODE_LINEAR_DEPTH = 2u;

// Scene visualization modes.
const uint VISUALIZATION_MODE_INSTANCE_INDEX = 3u;
const uint VISUALIZATION_MODE_GEOMETRY_INDEX = 4u;
const uint VISUALIZATION_MODE_MATERIAL_INDEX = 5u;

// Primitive visualization modes.
const uint VISUALIZATION_MODE_TRIANGLE_NORMAL = 6u;
const uint VISUALIZATION_MODE_BARYCENTRIC_DERIVATIVES = 7u;
const uint VISUALIZATION_MODE_VERTEX_NORMAL = 8u;
const uint VISUALIZATION_MODE_VERTEX_TEXTURE_COORDINATE = 9u;

// Shading visualization modes.
const uint VISUALIZATION_MODE_FLAT_SHADING = 10u;
const uint VISUALIZATION_MODE_BLINN_PHONG_SHADING = 11u;

// TODO: Use a value from a uniform buffer.
const uint VISUALIZATION_MODE = VISUALIZATION_MODE_BLINN_PHONG_SHADING;

layout(binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};
layout(binding = 1, r32ui) uniform readonly uimage2D visibilityImage;
layout(binding = 2) uniform sampler2D depthStencilImage;
layout(binding = 3) buffer readonly VertexPositionComponentBuffer {
	float vertexPositionComponentBuffer[];
};
layout(binding = 4) buffer readonly VertexNormalComponentBuffer {
	float vertexNormalComponentBuffer[];
};
layout(binding = 5) buffer readonly VertexTextureCoordinateComponentBuffer {
	float vertexTextureCoordinateComponentBuffer[];
};
layout(binding = 6) buffer readonly VertexPositionIndexBuffer {
	uint vertexPositionIndexBuffer[];
};
layout(binding = 7) buffer readonly VertexNormalIndexBuffer {
	uint vertexNormalIndexBuffer[];
};
layout(binding = 8) buffer readonly VertexTextureCoordinateIndexBuffer {
	uint vertexTextureCoordinateIndexBuffer[];
};
layout(binding = 9, rgba8) uniform writeonly image2D outputImage;
layout(binding = 12) buffer readonly ClusterBuffer {
	Cluster clusterBuffer[];
};
layout(binding = 13) buffer readonly InstanceBuffer {
	Instance instanceBuffer[];
};
layout(binding = 14) buffer readonly GeometryBuffer {
	Geometry geometryBuffer[];
};

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
	uint offset = (uint(geometry.vertexOffset / 3u * 2u) + index) * 2u;

	float x = vertexTextureCoordinateComponentBuffer[offset + 0u];
	float y = vertexTextureCoordinateComponentBuffer[offset + 1u];

	return vec2(x, y);
}

Triangle loadTriangle(uint triangleIndex, Instance instance, Geometry geometry) {
	// TODO: Is the conversion on instance.clusterOffset correct?
	uint indexOffset = uint(instance.clusterOffset) * CLUSTER_INDEX_COUNT + triangleIndex * 3u;

	Triangle triangle;

	triangle.vertices[0].position = loadVertexPosition(geometry, indexOffset + 0u);
	triangle.vertices[1].position = loadVertexPosition(geometry, indexOffset + 1u);
	triangle.vertices[2].position = loadVertexPosition(geometry, indexOffset + 2u);

	triangle.vertices[0].normal = loadVertexNormal(geometry, indexOffset + 0u);
	triangle.vertices[1].normal = loadVertexNormal(geometry, indexOffset + 1u);
	triangle.vertices[2].normal = loadVertexNormal(geometry, indexOffset + 2u);

	triangle.vertices[0].textureCoordinate = loadVertexTextureCoordinate(geometry, indexOffset + 0u);
	triangle.vertices[1].textureCoordinate = loadVertexTextureCoordinate(geometry, indexOffset + 1u);
	triangle.vertices[2].textureCoordinate = loadVertexTextureCoordinate(geometry, indexOffset + 2u);

	return triangle;
}

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

vec2 ndc(vec2 coordinate) {
	vec2 ndc = coordinate / vec2(camera.viewport) * 2.0 - 1.0;
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
BarycentricDerivatives computePartialDerivatives(vec2 a, vec2 b, vec2 c) {
	float dInverse = 1.0 / determinant(mat2x2(c - b, a - b));

	BarycentricDerivatives derivatives;
	derivatives.ddx = vec3(b.y - c.y, c.y - a.y, a.y - b.y) * dInverse;
	derivatives.ddy = vec3(c.x - b.x, a.x - c.x, b.x - a.x) * dInverse;

	return derivatives;
}

// `p` is the fragment coordinate in NDC space.
BarycentricDerivatives computeBarycentricDerivatives(Triangle triangle, Instance instance, vec2 p) {
	mat4x4 projectionViewTransform = camera.projectionView * instance.transform;
	vec4 a = projectionViewTransform * vec4(triangle.vertices[0].position, 1.0);
	vec4 b = projectionViewTransform * vec4(triangle.vertices[1].position, 1.0);
	vec4 c = projectionViewTransform * vec4(triangle.vertices[2].position, 1.0);

	vec3 wInverse = 1.0 / vec3(a.w, b.w, c.w);

	vec2 pA = a.xy * wInverse.x;
	vec2 pB = b.xy * wInverse.y;
	vec2 pC = c.xy * wInverse.z;

	BarycentricDerivatives derivatives = computePartialDerivatives(pA, pB, pC);

	derivatives.ddx *= wInverse;
	derivatives.ddy *= wInverse;

	float ddxSum = dot(derivatives.ddx, vec3(1.0));
	float ddySum = dot(derivatives.ddy, vec3(1.0));

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
	return derivatives.lambda.x * a + derivatives.lambda.y * b + derivatives.lambda.z * c;
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

vec3 visualizeTriangleNormal(Triangle triangle, Instance instance) {
	vec3 a = triangle.vertices[0].position;
	vec3 b = triangle.vertices[1].position;
	vec3 c = triangle.vertices[2].position;

	// TODO: Retrieve the normal matrix from the instance.
	mat4x4 T = instance.transform;
	mat3x3 N = transpose(inverse(mat3x3(T)));

	return normalize(N * cross(b - a, c - a));
}

vec3 visualizeBarycentricDerivatives(Triangle triangle, Instance instance, vec2 coordinate) {
	BarycentricDerivatives derivatives = computeBarycentricDerivatives(triangle, instance, coordinate);

	return derivatives.lambda;
}

vec3 visualizeVertexNormal(Triangle triangle, Instance instance, vec2 coordinate) {
	BarycentricDerivatives derivatives = computeBarycentricDerivatives(triangle, instance, coordinate);

	// TODO: Retrieve the normal matrix from the instance.
	mat4x4 T = instance.transform;
	mat3x3 N = transpose(inverse(mat3x3(T)));

	return normalize(N * interpolate3x3(derivatives, triangle.vertices[0].normal, triangle.vertices[1].normal, triangle.vertices[2].normal));
}

vec3 visualizeVertexTextureCoordinate(Triangle triangle, Instance instance, vec2 coordinate) {
	BarycentricDerivatives derivatives = computeBarycentricDerivatives(triangle, instance, coordinate);
	Gradients gradients = interpolate3x2(derivatives, triangle.vertices[0].textureCoordinate, triangle.vertices[1].textureCoordinate, triangle.vertices[2].textureCoordinate);

	return vec3(gradients.value, 0.0);
}

vec3 visualizeFlatShading(Triangle triangle, Instance instance, vec2 coordinate) {
	BarycentricDerivatives derivatives = computeBarycentricDerivatives(triangle, instance, coordinate);

	vec3 surface = (instance.transform * vec4(interpolate3x3(derivatives, triangle.vertices[0].position, triangle.vertices[1].position, triangle.vertices[2].position), 1.0)).xyz;
	vec3 surfaceToCameraDirection = normalize(camera.position - surface);
	vec3 normal = visualizeTriangleNormal(triangle, instance);

	float diffuse = max(0.0, dot(surfaceToCameraDirection, normal));

	return vec3(diffuse);
}

vec3 visualizeBlinnPhongShading(Triangle triangle, Instance instance, vec2 coordinate) {
	Light DEFAULT_LIGHT = Light(
		camera.position,
		vec3(0.2, 0.2, 0.2),
		vec3(0.5, 0.5, 0.5),
		vec3(1.0, 1.0, 1.0)
	);

	Material DEFAULT_MATERIAL = Material(
		vec3(1.0, 1.0, 1.0),
		vec3(1.0, 1.0, 1.0),
		vec3(0.2, 0.2, 0.2),
		100.0
	);

	BarycentricDerivatives derivatives = computeBarycentricDerivatives(triangle, instance, coordinate);

	vec3 surface = (instance.transform * vec4(interpolate3x3(derivatives, triangle.vertices[0].position, triangle.vertices[1].position, triangle.vertices[2].position), 1.0)).xyz;
	vec3 normal = visualizeVertexNormal(triangle, instance, coordinate);

	// Light position towards surface.
	vec3 lightDirection = normalize(DEFAULT_LIGHT.position - surface);

	// Camera position towards surface.
	vec3 cameraDirection = normalize(camera.position - surface);

	vec3 halfwayDirection = normalize(lightDirection + cameraDirection);

	float diffuseComponent = max(0.0, dot(normal, lightDirection));
	float specularComponent = pow(max(0.0, dot(normal, halfwayDirection)), DEFAULT_MATERIAL.shininess) * MATERIAL_SPECULAR_MULTIPLIER;

	vec3 ambient = DEFAULT_LIGHT.ambient * DEFAULT_MATERIAL.ambient;
	vec3 diffuse = DEFAULT_LIGHT.diffuse * DEFAULT_MATERIAL.diffuse * diffuseComponent;
	vec3 specular = DEFAULT_LIGHT.specular * DEFAULT_MATERIAL.specular * specularComponent;

	return ambient + diffuse + specular;
}

vec4 VisibilityDebug(uvec2 screenCoordinate) {
	uint visibility = imageLoad(visibilityImage, ivec2(screenCoordinate.x, camera.viewport.y - 1u - screenCoordinate.y)).r;

	if (visibility == 0u) {
		if (VISUALIZATION_MODE == VISUALIZATION_MODE_MATERIAL_INDEX) {
			return vec4(uintToColor(1u), 1.0);
		}

		if (VISUALIZATION_MODE == VISUALIZATION_MODE_LINEAR_DEPTH) {
			return vec4(1.0, 1.0, 1.0, 1.0);
		}

		return vec4(0.0, 0.0, 0.0, 1.0);
	}

	// Fragment coordinate in NDC space.
	vec2 coordinate = ndc(vec2(screenCoordinate));

	uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1u;

	// Triangle index relative to the parent cluster.
	uint clusterTriangleIndex = visibility & TRIANGLE_INDEX_BIT_MASK;

	// Global triangle index.
	uint triangleIndex = clusterIndex * CLUSTER_TRIANGLE_COUNT + clusterTriangleIndex;

	Cluster cluster = clusterBuffer[clusterIndex];
	Instance instance = instanceBuffer[cluster.instanceIndex];
	Geometry geometry = geometryBuffer[instance.geometryIndex];

	Triangle triangle = loadTriangle(triangleIndex, instance, geometry);

	vec3 color;

	switch (VISUALIZATION_MODE) {
		case VISUALIZATION_MODE_TRIANGLE_INDEX: {
			return vec4(uintToColor(triangleIndex + 1u), 1.0);
		}
		case VISUALIZATION_MODE_CLUSTER_INDEX: {
			return vec4(uintToColor(clusterIndex + 1u), 1.0);
		}
		case VISUALIZATION_MODE_LINEAR_DEPTH: {
			float nonLinearDepth = texture(depthStencilImage, vec2(screenCoordinate.x, camera.viewport.y - 1u - screenCoordinate.y) / vec2(camera.viewport)).r;
			float linearDepth = linearizeDepth(nonLinearDepth);

			return vec4(vec3(linearDepth), 1.0);
		}
		case VISUALIZATION_MODE_INSTANCE_INDEX: {
			return vec4(uintToColor(cluster.instanceIndex + 1u), 1.0);
		}
		case VISUALIZATION_MODE_GEOMETRY_INDEX: {
			return vec4(uintToColor(instance.geometryIndex + 1u), 1.0);
		}
		case VISUALIZATION_MODE_MATERIAL_INDEX: {
			return vec4(uintToColor(instance.materialShaderIndex + 1u), 1.0);
		}
		case VISUALIZATION_MODE_TRIANGLE_NORMAL: {
			return vec4(visualizeTriangleNormal(triangle, instance), 1.0);
		}
		case VISUALIZATION_MODE_BARYCENTRIC_DERIVATIVES: {
			return vec4(visualizeBarycentricDerivatives(triangle, instance, coordinate), 1.0);
		}
		case VISUALIZATION_MODE_VERTEX_NORMAL: {
			return vec4(visualizeVertexNormal(triangle, instance, coordinate), 1.0);
		}
		case VISUALIZATION_MODE_VERTEX_TEXTURE_COORDINATE: {
			return vec4(visualizeVertexTextureCoordinate(triangle, instance, coordinate), 1.0);
		}
		case VISUALIZATION_MODE_FLAT_SHADING: {
			return vec4(visualizeFlatShading(triangle, instance, coordinate), 1.0);
		}
		case VISUALIZATION_MODE_BLINN_PHONG_SHADING: {
			return vec4(visualizeBlinnPhongShading(triangle, instance, coordinate), 1.0);
		}
		default: {
			vec4(0.0, 0.0, 0.0, 1.0);
		}
	}
}

void main() {
	uvec2 screenCoordinate = gl_GlobalInvocationID.xy;

	if (screenCoordinate.x >= camera.viewport.x || screenCoordinate.y >= camera.viewport.y) {
		return;
	}

	vec4 color = VisibilityDebug(screenCoordinate);

	imageStore(outputImage, ivec2(screenCoordinate), color);
}