// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"
#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	void MapBuilder::triangulateFaces(IntermediateGeometry& geometry, uint32 lineIndexOffset) {
		uint32 vertexOffset = 0;
		uint32 triangleOffset = 0;
		uint32 triangleIndexOffset = 0;
		uint32 faceIndex = 0;

		while (faceIndex < geometry.faceIndices.size()) {
			Face& face = faces[geometry.faceIndices[faceIndex]];
			face.vertexOffset = vertexOffset;
			face.triangleOffset = triangleOffset;

			const uint32 indexCount = face.getIndexCount();
			const vec2<float32> faceSize = face.boundingBox.max - face.boundingBox.min;
			const mat3x3<float32> textureTransform = (
				scaling<mat3x3<float32>>(faceSize / face.textureFiltering.scale) *
				rotationAngle<mat3x3<float32>>(face.textureFiltering.rotation * pi<float32>()) *
				translation<mat3x3<float32>>(face.textureFiltering.translation)
			);
			const quat<float32> R = lookAt(face.normal, {0.0f, 0.0f, -1.0f}, UP);

			std::vector<uint32> faceIndices2(face.getUniqueIndexCount());

			// Final index buffer for the face, with indices in [0, indexCount[.
			std::vector<uint32> indexBuffer(indexCount);

			// TODO(asset-builder/map/face-triangulation): Refactor.
			{
				uint32 i = 0;
				uint32 indexCount2 = 0;

				while (i < indexCount) {
					const uint32 index = face.indices[i];

					bool duplicate = false;
					uint32 j = 0;

					while (j < indexCount2) {
						const uint32 index2 = faceIndices2[j];

						if (index == index2) {
							duplicate = true;

							break;
						}

						j += 1;
					}

					if (!duplicate) {
						faceIndices2[indexCount2] = index;
						indexCount2 += 1;
					}

					indexBuffer[i] = j;

					i += 1;
				}
			}

			uint32 vertexIndex = 0;

			while (vertexIndex < face.getUniqueIndexCount()) {
				BuildVertex3D& vertex = geometry.vertices[vertexOffset + vertexIndex];
				vertex.position = facePositions[faceIndices2[vertexIndex]];

				const quat<float32> q = R * quat<float32>(0.0f, vertex.position.x, vertex.position.y, vertex.position.z) * inverse(R);

				vec2<float32> textureCoordinate = {q.x, q.y};
				textureCoordinate -= face.boundingBox.min;
				textureCoordinate /= faceSize;
				textureCoordinate.y = 1.0f - textureCoordinate.y;

				const uint32 initialTextureCoordinate = pack<VertexInputAttributeFormat::R16G16_FLOAT, uint32>(textureCoordinate);

				vertex.normal = face.normal;
				vertex.tangent = {0.0f, 0.0f, 0.0f, *pointer_cast<const float32*>(&initialTextureCoordinate)};
				vertex.textureCoordinate = multiplyTransform(textureCoordinate, textureTransform);

				// TODO(asset-builder/map/static-geometry): Restore face wireframe?
				// scene.staticGeometry.lineIndices[lineIndexOffset + vertexIndex * 2 + 0] = face.indexRange.offset + vertexIndex;
				// scene.staticGeometry.lineIndices[lineIndexOffset + vertexIndex * 2 + 1] = face.indexRange.offset + (vertexIndex + 1) % indexCount;

				vertexIndex += 1;
			}

			std::vector<uint32> indicesCopy = face.indices;

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

					if (abc >= 0.0f) {
						indexOffset += 1;

						continue;
					}

					bool pointInside = false;
					uint32 i = 0;

					while (i < indicesCopy.size()) {
						const vec3<float32> p = facePositions[indicesCopy[i]];

						if (pointTriangleIntersection(a, b, c, p)) {
							pointInside = true;

							break;
						}

						i += 1;
					}

					if (pointInside) {
						indexOffset += 1;

						continue;
					}

					geometry.indices[triangleIndexOffset + 0] = vertexOffset + indexBuffer[indexOffsetA];
					geometry.indices[triangleIndexOffset + 1] = vertexOffset + indexBuffer[indexOffsetB];
					geometry.indices[triangleIndexOffset + 2] = vertexOffset + indexBuffer[indexOffsetC];

					indicesCopy.erase(indicesCopy.begin() + indexOffsetB);
					indexBuffer.erase(indexBuffer.begin() + indexOffsetB);

					triangleIndexOffset += 3;

					break;
				}
			}

			vertexOffset += face.getUniqueIndexCount();
			triangleOffset += face.getTriangleCount();
			// lineIndexOffset += indexCount * 2;
			faceIndex += 1;
		}
	}
}