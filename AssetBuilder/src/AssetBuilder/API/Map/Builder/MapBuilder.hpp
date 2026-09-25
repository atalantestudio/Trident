// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	Face& SceneBuilder::getFace(uint32 index) {
		return faces[index];
	}

	void SceneBuilder::extrudeFace(Face& face, float32 scale) {
		if (abs(scale) < 1e-4) {
			return;
		}

		const uint32 materialShaderIndex = face.materialReference >> 16;
		const uint32 materialIndex = face.materialReference & 0xff;

		const IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];
		const IntermediateMaterial& material = materialShader.materials[materialIndex];
		const IntermediateMaterialGeometry& materialGeometry = material.geometries[material.bakedGeometryIndex];

		IntermediateGeometry& geometry = geometries[materialGeometry.geometryIndex];

		vec3<float32> a = facePositions[indices[face.indexRange.offset]];
		uint32 indexA = 0;

		while (indexA < face.vertexCount) {
			// TODO(lyah/scalar)
			facePositions[facePositionCount + indexA] = svec3<float32>(a) + face.normal * scale;

			const uint32 indexB = (indexA + 1) % face.vertexCount;
			const vec3<float32> b = facePositions[indices[face.indexRange.offset + indexB]];

			Face extrudedFace{};
			extrudedFace.vertexCount = 4;
			extrudedFace.indexRange.offset = staticGeometryFaceVertexCount + indexA * 4;

			// TODO(lyah/scalar)
			extrudedFace.normal = svec3<float32>(normalized(cross(b - a, face.normal * scale)));

			extrudedFace.geometryIndex = face.geometryIndex;
			extrudedFace.materialReference = face.materialReference;
			extrudedFace.lightmap = face.lightmap;
			extrudedFace.boundingBox.min = {max<float32>(), max<float32>()};
			extrudedFace.boundingBox.max = {min<float32>(), min<float32>()};

			indices[extrudedFace.indexRange.offset + 0] = facePositionCount + indexB;
			indices[extrudedFace.indexRange.offset + 1] = facePositionCount + indexA;
			indices[extrudedFace.indexRange.offset + 2] = indices[face.indexRange.offset + indexA];
			indices[extrudedFace.indexRange.offset + 3] = indices[face.indexRange.offset + indexB];

			faces[staticGeometryFaceCount] = extrudedFace;

			geometry.faceIndices.emplace_back(staticGeometryFaceCount);

			a = b;

			staticGeometryFaceCount += 1;
			indexA += 1;
		}

		indexA = 0;

		while (indexA < face.vertexCount) {
			indices[face.indexRange.offset + indexA] = facePositionCount + indexA;

			indexA += 1;
		}

		facePositionCount += face.vertexCount;
		staticGeometryFaceVertexCount += face.vertexCount * 4;
		staticGeometryFaceTriangleCount += face.vertexCount * 2;
		lightmapCount += face.vertexCount;
	}
}