#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) vec3 position;
	layout(location = 1) flat uint triangleIndex;
	layout(location = 2) flat vec3 triangleNormal;
	layout(location = 3) vec3 vertexNormal;
	layout(location = 4) flat vec3 positionA;
	layout(location = 5) flat vec3 positionB;
	layout(location = 6) flat vec3 positionC;
	layout(location = 7) flat vec3 normalA;
	layout(location = 8) flat vec3 normalB;
	layout(location = 9) flat vec3 normalC;
	layout(location = 10) vec3 barycentricCoordinate;
	layout(location = 11) flat uint materialIndex;
	layout(location = 12) flat float triangleArea;
} interstage;

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 smoothPosition;
layout(location = 2) out vec4 triangleNormal;
layout(location = 3) out vec4 vertexNormal;

vec3 planeProjection(vec3 p, vec3 offset, vec3 normal) {
	return p - dot(p - offset, normal) * normal;
}

void main() {
	vec3 b = interstage.barycentricCoordinate;
	vec3 p = interstage.position;
	vec3 luxelSize = max(abs(dFdx(p)), abs(dFdy(p)));
	float halfDiagonalLuxelSize = max(max(luxelSize.x, luxelSize.y), luxelSize.z) * sqrt(2.0) * 0.5;

	position = vec4(p, interstage.triangleIndex);

	vec3 projectA = planeProjection(p, interstage.positionA, interstage.normalA);
	vec3 projectB = planeProjection(p, interstage.positionB, interstage.normalB);
	vec3 projectC = planeProjection(p, interstage.positionC, interstage.normalC);
	vec3 p2 = b.x * projectA + b.y * projectB + b.z * projectC;

	// TODO(asset-builder/lightmapper): Find a good interpolant.
	smoothPosition = vec4(mix(p2, p, 0.0001), interstage.triangleArea);

	triangleNormal = vec4(normalize(interstage.triangleNormal), halfDiagonalLuxelSize);
	vertexNormal = vec4(normalize(interstage.vertexNormal), float(interstage.materialIndex));
}