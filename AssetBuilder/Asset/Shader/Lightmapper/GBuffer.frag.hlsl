// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 coordinate : SV_Position;
	float3 position : VERTEX_OUTPUT_ATTRIBUTE1;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE2;
	nointerpolation float3 triangleNormal : VERTEX_OUTPUT_ATTRIBUTE3;
	float3 vertexNormal : VERTEX_OUTPUT_ATTRIBUTE4;
	nointerpolation float3 positionA : VERTEX_OUTPUT_ATTRIBUTE5;
	nointerpolation float3 positionB : VERTEX_OUTPUT_ATTRIBUTE6;
	nointerpolation float3 positionC : VERTEX_OUTPUT_ATTRIBUTE7;
	nointerpolation float3 normalA : VERTEX_OUTPUT_ATTRIBUTE8;
	nointerpolation float3 normalB : VERTEX_OUTPUT_ATTRIBUTE9;
	nointerpolation float3 normalC : VERTEX_OUTPUT_ATTRIBUTE10;
	float3 barycentricCoordinate : VERTEX_OUTPUT_ATTRIBUTE11;
	nointerpolation uint materialIndex : VERTEX_OUTPUT_ATTRIBUTE12;
};

struct FragmentOutput {
	float4 position : SV_Target0;
	float4 smoothPosition : SV_Target1;
	float4 triangleNormal : SV_Target2;
	float4 vertexNormal : SV_Target3;
};

float3 planeProjection(float3 p, float3 offset, float3 normal) {
	return p - dot(p - offset, normal) * normal;
}

FragmentOutput main(Interstage interstage) {
	float3 b = interstage.barycentricCoordinate;
	float3 p = interstage.position;
	float3 luxelSize = max(abs(ddx(p)), abs(ddy(p)));
	float diagonalLuxelSize = max(max(luxelSize.x, luxelSize.y), luxelSize.z) * sqrt(2.0);

	FragmentOutput output;
	output.position = float4(p, interstage.triangleIndex);

	float3 projectA = planeProjection(p, interstage.positionA, interstage.normalA);
	float3 projectB = planeProjection(p, interstage.positionB, interstage.normalB);
	float3 projectC = planeProjection(p, interstage.positionC, interstage.normalC);
	float3 p2 = b.x * projectA + b.y * projectB + b.z * projectC;

	// TODO(asset-builder/lightmapper): Find a good interpolant.
	output.smoothPosition = float4(lerp(p2, p, 0.0001), 0.0);

	output.triangleNormal = float4(normalize(interstage.triangleNormal), diagonalLuxelSize);
	output.vertexNormal = float4(normalize(interstage.vertexNormal), interstage.materialIndex);

	return output;
}