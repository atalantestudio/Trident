// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 coordinate : SV_Position;
	float3 position : VERTEX_OUTPUT_ATTRIBUTE1;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE2;
	nointerpolation float3 triangleNormal : VERTEX_OUTPUT_ATTRIBUTE3;
	float3 vertexNormal : VERTEX_OUTPUT_ATTRIBUTE4;
};

struct FragmentOutput {
	float4 position : SV_Target0;
	float4 triangleNormal : SV_Target1;
	float4 vertexNormal : SV_Target2;
};

FragmentOutput main(Interstage interstage) {
	float3 luxelSize = max(abs(ddx(interstage.position)), abs(ddy(interstage.position)));
	float diagonalLuxelSize = max(max(luxelSize.x, luxelSize.y), luxelSize.z) * sqrt(2.0);

	FragmentOutput output;
	output.position = float4(interstage.position, interstage.triangleIndex);
	output.triangleNormal = float4(interstage.triangleNormal, diagonalLuxelSize);
	output.vertexNormal = float4(interstage.vertexNormal, 0.0);

	return output;
}