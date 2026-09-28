// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	void MapBuilder::extrudeFace(Face& face, float32 scale) {
		if (abs(scale) < 1e-4) {
			return;
		}

		const uint32 vertexCount = face.getVertexCount();
		const uint32 materialShaderIndex = face.material >> 16;
		const uint32 materialIndex = face.material & 0xff;

		const IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];
		const IntermediateMaterial& material = materialShader.materials[materialIndex];
		const IntermediateMaterialGeometry& materialGeometry = material.geometries[material.bakedGeometryIndex];

		IntermediateGeometry& geometry = geometries[materialGeometry.geometryIndex];

		vec3<float32> a = facePositions[face.indices[0]];
		uint32 indexA = 0;

		while (indexA < vertexCount) {
			// TODO(lyah/scalar)
			facePositions[facePositionCount + indexA] = svec3<float32>(a) + face.normal * scale;

			const uint32 indexB = (indexA + 1) % vertexCount;
			const vec3<float32> b = facePositions[face.indices[indexB]];

			Face extrudedFace{};
			extrudedFace.indices = {
				facePositionCount + indexB,
				facePositionCount + indexA,
				face.indices[indexA],
				face.indices[indexB],
			};

			// TODO(lyah/scalar)
			extrudedFace.normal = svec3<float32>(normalized(cross(b - a, face.normal * scale)));

			extrudedFace.geometryIndex = face.geometryIndex;
			extrudedFace.material = face.material;
			extrudedFace.textureFiltering = face.textureFiltering;
			extrudedFace.lightmap = face.lightmap;
			extrudedFace.boundingBox.min = {max<float32>(), max<float32>()};
			extrudedFace.boundingBox.max = {min<float32>(), min<float32>()};

			faces[staticGeometryFaceCount] = extrudedFace;

			geometry.faceIndices.emplace_back(staticGeometryFaceCount);

			a = b;

			staticGeometryFaceCount += 1;
			staticGeometryFaceTriangleCount += 2;
			lightmapCoordinateCount += vertexCount;
			indexA += 1;
		}

		indexA = 0;

		while (indexA < vertexCount) {
			face.indices[indexA] = facePositionCount + indexA;

			indexA += 1;
		}

		facePositionCount += vertexCount;
		staticGeometryFaceVertexCount += vertexCount * 4;
		lightmapCount += vertexCount;
	}
}