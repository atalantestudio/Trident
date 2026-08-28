// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	/* void MapBuilder::triangulateFaces(BuildScene3D& scene) const {
		uint32 faceIndex = 0;
		uint32 vertexOffset = 0;
		uint32 indexOffset = 0;
		uint32 lineIndexOffset = 0;

		while (faceIndex < faceCount) {
			const Face& face = faces[faceIndex];
			const uint32 triangleCount = face.vertexCount - 2;

			uint32 vertexIndex = 0;
			uint32 triangleIndex = 0;

			while (vertexIndex < face.vertexCount) {
				BuildVertex3D& vertex = scene.staticGeometry.bakedVertices[vertexOffset + vertexIndex];
				vertex.position = facePositions[indices[face.indexRange.offset + vertexIndex]];
				vertex.normal = face.normal;

				vertex.tangent = {0.0f, 0.0f, 0.0f, 0.0f};
				vertex.textureCoordinate = {0.0f, 0.0f};

				scene.staticGeometry.lineIndices[lineIndexOffset + vertexIndex * 2 + 0] = face.indexRange.offset + vertexIndex;
				scene.staticGeometry.lineIndices[lineIndexOffset + vertexIndex * 2 + 1] = face.indexRange.offset + (vertexIndex + 1) % face.vertexCount;

				vertexIndex += 1;
			}

			while (triangleIndex < triangleCount) {
				scene.staticGeometry.bakedIndices[indexOffset + triangleIndex * 3 + 0] = face.indexRange.offset;
				scene.staticGeometry.bakedIndices[indexOffset + triangleIndex * 3 + 1] = face.indexRange.offset + triangleIndex + 1;
				scene.staticGeometry.bakedIndices[indexOffset + triangleIndex * 3 + 2] = face.indexRange.offset + triangleIndex + 2;

				triangleIndex += 1;
			}

			vertexOffset += vertexIndex;
			indexOffset += triangleCount * 3;
			lineIndexOffset += face.vertexCount * 2;
			faceIndex += 1;
		}
	} */

	void MapBuilder::triangulateFaces(BuildScene3D& scene) const {
		uint32 faceIndex = 0;
		uint32 vertexOffset = 0;
		uint32 triangleIndexOffset = 0;
		uint32 lineIndexOffset = 0;

		while (faceIndex < faceCount) {
			const Face& face = faces[faceIndex];

			uint32 vertexIndex = 0;

			while (vertexIndex < face.vertexCount) {
				BuildVertex3D& vertex = scene.staticGeometry.bakedVertices[vertexOffset + vertexIndex];
				vertex.position = facePositions[indices[face.indexRange.offset + vertexIndex]];
				vertex.normal = face.normal;

				// TODO(asset-builder/map): Calculate face tangent and vertex texture coordinates.
				vertex.tangent = {0.0f, 0.0f, 0.0f, 0.0f};
				vertex.textureCoordinate = {0.0f, 0.0f};

				scene.staticGeometry.lineIndices[lineIndexOffset + vertexIndex * 2 + 0] = face.indexRange.offset + vertexIndex;
				scene.staticGeometry.lineIndices[lineIndexOffset + vertexIndex * 2 + 1] = face.indexRange.offset + (vertexIndex + 1) % face.vertexCount;

				vertexIndex += 1;
			}

			std::vector<uint32> indicesCopy(face.vertexCount);

			// Final index buffer for the face, with indices in [0, face.vertexCount].
			std::vector<uint32> indexBuffer(face.vertexCount);

			std::copy_n(&indices[face.indexRange.offset], face.vertexCount, &indicesCopy[0]);

			uint32 i = 0;

			while (i < face.vertexCount) {
				indexBuffer[i] = i;

				i += 1;
			}

			while (indicesCopy.size() > 2) {
				uint32 indexOffset = 0;

				while (indexOffset < indicesCopy.size()) {
					const uint32 indexOffsetA = indexOffset;
					const uint32 indexOffsetB = (indexOffset + 1) % indicesCopy.size();
					const uint32 indexOffsetC = (indexOffset + 2) % indicesCopy.size();

					const uint32 indexA = indicesCopy[indexOffsetA];
					const uint32 indexB = indicesCopy[indexOffsetB];
					const uint32 indexC = indicesCopy[indexOffsetC];

					const vec3<float32> a = facePositions[indexA];
					const vec3<float32> b = facePositions[indexB];
					const vec3<float32> c = facePositions[indexC];

					const float32 abc = dot(cross(b - a, face.normal), c - b);

					ENGINE_TRACE("[]/[]/[], angle = []", indexA, indexB, indexC, abc);

					if (abc < 0.0f) {
						// TODO(asset-builder/map/triangulation): Check if the triangle is fully inside the face.
						// TODO(asset-builder/map): Handle holes.

						scene.staticGeometry.bakedIndices[triangleIndexOffset + 0] = vertexOffset + indexBuffer[indexOffsetA];
						scene.staticGeometry.bakedIndices[triangleIndexOffset + 1] = vertexOffset + indexBuffer[indexOffsetB];
						scene.staticGeometry.bakedIndices[triangleIndexOffset + 2] = vertexOffset + indexBuffer[indexOffsetC];

						indicesCopy.erase(indicesCopy.begin() + indexOffsetB);
						indexBuffer.erase(indexBuffer.begin() + indexOffsetB);

						triangleIndexOffset += 3;

						break;
					}

					indexOffset += 1;
				}
			}

			vertexOffset += face.vertexCount;
			lineIndexOffset += face.vertexCount * 2;
			faceIndex += 1;
		}
	}
}