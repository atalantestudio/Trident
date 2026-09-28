// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	Edge2D MapBuilder::findClosestFaceCarvingConnector(const Face& face, view<svec3<float32>> carvingVertices) const {
		const uint32 faceVertexCount = face.getVertexCount();

		float32 minDistance = max<float32>();
		Edge2D connector{};
		uint32 carvingVertexIndex = 0;

		while (carvingVertexIndex < carvingVertices.count()) {
			const vec3<float32> carvingVertex = carvingVertices[carvingVertexIndex];

			uint32 faceVertexIndex = 0;

			while (faceVertexIndex < faceVertexCount) {
				const vec3<float32> faceVertex = facePositions[face.indices[faceVertexIndex]];
				const float32 distance = distanceSquared(carvingVertex, faceVertex);

				if (distance < minDistance) {
					minDistance = distance;
					connector.indexA = faceVertexIndex;
					connector.indexB = carvingVertexIndex;
				}

				faceVertexIndex += 1;
			}

			carvingVertexIndex += 1;
		}

		return connector;
	}

	Face& MapBuilder::carveFace(Face& face, view<svec3<float32>> carvingVertices) {
		const Edge2D connector = findClosestFaceCarvingConnector(face, carvingVertices);

		sequence<uint32> carvingIndices(carvingVertices.count());
		uint32 vertexCount = face.getVertexCount();
		uint32 carvingVertexIndex = 0;

		while (carvingVertexIndex < carvingVertices.count()) {
			facePositions[facePositionCount + carvingVertexIndex] = carvingVertices[carvingVertexIndex];
			carvingIndices[carvingVertexIndex] = facePositionCount + carvingVertexIndex;

			carvingVertexIndex += 1;
		}

		const uint32 facePositionOffset = facePositionCount;

		facePositionCount += static_cast<uint32>(carvingVertices.count());
		lightmapCoordinateCount -= vertexCount;

		vertexCount += static_cast<uint32>(carvingVertices.count()) + 2;

		face.indices.resize(vertexCount);

		uint32 index = connector.indexA + static_cast<uint32>(carvingVertices.count()) + 2;
		uint32 j = 0;

		while (index < vertexCount) {
			face.indices[index] = face.indices[j];

			index += 1;
			j += 1;
		}

		index = connector.indexA + 1;

		face.indices[index] = facePositionOffset + connector.indexB;

		index += 1;
		j = 0;

		while (j < carvingVertices.count()) {
			face.indices[index] = facePositionCount - 1 - j;

			carvingIndices[j] = facePositionOffset + j;

			index += 1;
			j += 1;
		}

		lightmapCoordinateCount += vertexCount;

		IndexedFaceDescriptor descriptor{};
		descriptor.indices = carvingIndices;
		descriptor.normal = face.normal;
		descriptor.material = face.material;
		descriptor.textureFiltering = face.textureFiltering;
		descriptor.lightmap = face.lightmap;

		return createFace(descriptor);
	}
}