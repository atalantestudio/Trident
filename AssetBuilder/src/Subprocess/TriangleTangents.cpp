// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "TriangleTangents.hpp"

namespace Trident {
	sequence<float32> calculateTriangleTangentComponents(
		view<float32> vertexPositionComponents,
		view<float32> vertexTextureCoordinateComponents,
		view<uint32> vertexPositionIndices,
		view<uint32> vertexTextureCoordinateIndices
	) {
		const index triangleTangentComponentCount = vertexPositionIndices.count();
		const index triangleCount = triangleTangentComponentCount / 3;

		sequence<float32> triangleTangentComponents(triangleTangentComponentCount);
		index triangleIndex = 0;

		// TODO(geometry/triangle-tangents): Good candidate for SIMD optimizations.
		while (triangleIndex < triangleCount) {
			const uint32 index_pA = vertexPositionIndices[triangleIndex * 3 + 0];
			const uint32 index_pB = vertexPositionIndices[triangleIndex * 3 + 1];
			const uint32 index_pC = vertexPositionIndices[triangleIndex * 3 + 2];
			const uint32 index_uvA = vertexTextureCoordinateIndices[triangleIndex * 3 + 0];
			const uint32 index_uvB = vertexTextureCoordinateIndices[triangleIndex * 3 + 1];
			const uint32 index_uvC = vertexTextureCoordinateIndices[triangleIndex * 3 + 2];

			const vec3<float32> pA = *pointer_cast<const svec3<float32>*>(&vertexPositionComponents[index_pA * 3]);
			const vec3<float32> pB = *pointer_cast<const svec3<float32>*>(&vertexPositionComponents[index_pB * 3]);
			const vec3<float32> pC = *pointer_cast<const svec3<float32>*>(&vertexPositionComponents[index_pC * 3]);
			const vec2<float32> uvA = *pointer_cast<const svec2<float32>*>(&vertexTextureCoordinateComponents[index_uvA * 2]);
			const vec2<float32> uvB = *pointer_cast<const svec2<float32>*>(&vertexTextureCoordinateComponents[index_uvB * 2]);
			const vec2<float32> uvC = *pointer_cast<const svec2<float32>*>(&vertexTextureCoordinateComponents[index_uvC * 2]);

			const vec3<float32> delta_pB = pB - pA;
			const vec3<float32> delta_pC = pC - pA;
			const vec2<float32> delta_uvB = uvB - uvA;
			const vec2<float32> delta_uvC = uvC - uvA;
			const float32 f = 1.0f / (delta_uvB[0] * delta_uvC[1] - delta_uvC[0] * delta_uvB[1]);
			const vec3<float32> tangent = f * (delta_uvC[1] * delta_pB - delta_uvB[1] * delta_pC);

			triangleTangentComponents[triangleIndex * 3 + 0] = tangent[0];
			triangleTangentComponents[triangleIndex * 3 + 1] = tangent[1];
			triangleTangentComponents[triangleIndex * 3 + 2] = tangent[2];

			triangleIndex += 1;
		}

		return triangleTangentComponents;
	}
}