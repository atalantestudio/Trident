// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	static bool pointInsideTriangle(vec3<float32> a, vec3<float32> b, vec3<float32> c, vec3<float32> p) {
		const vec3<float32> ab = b - a;
		const vec3<float32> ac = c - a;
		const vec3<float32> ap = p - a;

		const float32 ab_ab = dot(ab, ab);
		const float32 ab_ac = dot(ab, ac);
		const float32 ac_ac = dot(ac, ac);
		const float32 ap_ab = dot(ap, ab);
		const float32 ap_ac = dot(ap, ac);

		const float32 inverseD = 1.0f / (ab_ab * ac_ac - ab_ac * ab_ac);

		const float32 v = (ac_ac * ap_ab - ab_ac * ap_ac) * inverseD;
		const float32 w = (ab_ab * ap_ac - ab_ac * ap_ab) * inverseD;

		return v > 0.0f && w > 0.0f;
	}

	void SceneBuilder::calculateFaceBoundingBoxes(const IntermediateGeometry& intermediateGeometry) {
		uint32 faceIndex = 0;

		while (faceIndex < intermediateGeometry.faceIndices.size()) {
			Face& face = faces[intermediateGeometry.faceIndices[faceIndex]];

			const quat<float32> R = lookAt(face.normal, {0.0f, 0.0f, -1.0f}, UP);

			uint32 vertexIndex = 0;

			while (vertexIndex < face.vertexCount) {
				svec3<float32> vertex = facePositions[indices[face.indexRange.offset + vertexIndex]];

				const quat<float32> q = {0.0f, vertex.x, vertex.y, vertex.z};

				vertex = svec3<float32>((R * q * inverse(R)).xyz());

				face.boundingBox.min.x = min(face.boundingBox.min.x, vertex.x);
				face.boundingBox.min.y = min(face.boundingBox.min.y, vertex.y);
				face.boundingBox.max.x = max(face.boundingBox.max.x, vertex.x);
				face.boundingBox.max.y = max(face.boundingBox.max.y, vertex.y);

				vertexIndex += 1;
			}

			faceIndex += 1;
		}
	}

	void SceneBuilder::triangulateFaces(BuildScene3D& scene, const IntermediateGeometry& intermediateGeometry, BuildGeometry& geometry, uint32 lineIndexOffset) const {
		uint32 vertexOffset = geometry.vertexRange.offset;
		uint32 triangleIndexOffset = geometry.indexRange.offset;
		uint32 faceIndex = 0;

		while (faceIndex < intermediateGeometry.faceIndices.size()) {
			const Face& face = faces[intermediateGeometry.faceIndices[faceIndex]];
			const svec2<float32> faceSize = face.boundingBox.max - face.boundingBox.min;
			const mat3x3<float32> textureTransform = (
				mat3x3<float32>::scaling(faceSize / face.textureFiltering.scale) *
				mat3x3<float32>::rotation(face.textureFiltering.rotation * pi<float32>()) *
				mat3x3<float32>::translation(face.textureFiltering.translation)
			);
			const quat<float32> R = lookAt(face.normal, {0.0f, 0.0f, -1.0f}, UP);

			geometry.vertexRange.count += face.vertexCount;
			geometry.indexRange.count += (face.vertexCount - 2) * 3;

			uint32 vertexIndex = 0;

			while (vertexIndex < face.vertexCount) {
				BuildVertex3D& vertex = scene.vertices[vertexOffset + vertexIndex];
				vertex.position = facePositions[indices[face.indexRange.offset + vertexIndex]];

				const quat<float32> q = {0.0f, vertex.position.x, vertex.position.y, vertex.position.z};
				const svec3<float32> coordinate = svec3<float32>((R * q * inverse(R)).xyz());

				svec2<float32> textureCoordinate = {coordinate.x, coordinate.y};
				textureCoordinate -= face.boundingBox.min;
				textureCoordinate /= faceSize;

				const uint32 initialTextureCoordinate = pack<VertexInputAttributeFormat::R16G16_FLOAT, uint32>(textureCoordinate);

				vertex.normal = face.normal;
				vertex.tangent = {0.0f, 0.0f, 0.0f, *pointer_cast<const float32*>(&initialTextureCoordinate)};
				vertex.textureCoordinate = svec2<float32>(multiplyTransform(vec2<float32>(textureCoordinate), textureTransform));
				vertex.textureCoordinate.y = 1.0f - vertex.textureCoordinate.y;

				// TODO(asset-builder/map/static-geometry): Restore face wireframe?
				// scene.staticGeometry.lineIndices[lineIndexOffset + vertexIndex * 2 + 0] = face.indexRange.offset + vertexIndex;
				// scene.staticGeometry.lineIndices[lineIndexOffset + vertexIndex * 2 + 1] = face.indexRange.offset + (vertexIndex + 1) % face.vertexCount;

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

					// ENGINE_TRACE("[]/[]/[], angle = []", indexA, indexB, indexC, abc);

					if (abc >= 0.0f) {
						indexOffset += 1;

						continue;
					}

					bool pointInside = false;
					uint32 i = 0;

					while (i < indicesCopy.size()) {
						const vec3<float32> p = facePositions[indicesCopy[i]];

						if (pointInsideTriangle(a, b, c, p)) {
							pointInside = true;

							break;
						}

						i += 1;
					}

					if (pointInside) {
						indexOffset += 1;

						continue;
					}

					// TODO(asset-builder/map): Handle holes.

					scene.indices[triangleIndexOffset + 0] = vertexOffset + indexBuffer[indexOffsetA];
					scene.indices[triangleIndexOffset + 1] = vertexOffset + indexBuffer[indexOffsetB];
					scene.indices[triangleIndexOffset + 2] = vertexOffset + indexBuffer[indexOffsetC];

					indicesCopy.erase(indicesCopy.begin() + indexOffsetB);
					indexBuffer.erase(indexBuffer.begin() + indexOffsetB);

					triangleIndexOffset += 3;

					break;
				}
			}

			vertexOffset += face.vertexCount;
			lineIndexOffset += face.vertexCount * 2;
			faceIndex += 1;
		}
	}
}