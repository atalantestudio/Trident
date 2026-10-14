// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	Edge2D MapBuilder::findClosestFaceCarvingConnectorEdge(const Face& face, view<vec3<float32>> carvingVertices) const {
		const uint32 faceIndexCount = face.getIndexCount();

		float32 minDistance = max<float32>();
		Edge2D connectorEdge{};
		uint32 carvingVertexIndex = 0;

		while (carvingVertexIndex < carvingVertices.count()) {
			const vec3<float32> carvingVertex = carvingVertices[carvingVertexIndex];

			uint32 faceIndexOffset = 0;

			while (faceIndexOffset < faceIndexCount) {
				const vec3<float32> faceVertex = facePositions[face.indices[faceIndexOffset]];
				const float32 distance = distanceSquared(carvingVertex, faceVertex);

				if (distance < minDistance) {
					minDistance = distance;
					connectorEdge.indexA = faceIndexOffset;
					connectorEdge.indexB = carvingVertexIndex;
				}

				faceIndexOffset += 1;
			}

			carvingVertexIndex += 1;
		}

		return connectorEdge;
	}

	Face& MapBuilder::carveFace(Face& face, view<vec3<float32>> carvingVertices) {
		const uint32 previousIndexCount = face.getIndexCount();
		const uint32 carvingVertexCount = static_cast<uint32>(carvingVertices.count());
		const Edge2D connectorEdge = findClosestFaceCarvingConnectorEdge(face, carvingVertices);

		sequence<uint32> carvingIndices(carvingVertexCount);
		uint32 carvingVertexIndex = 0;

		while (carvingVertexIndex < carvingVertexCount) {
			facePositions[facePositionCount + carvingVertexIndex] = carvingVertices[carvingVertexIndex];
			carvingIndices[carvingVertexIndex] = facePositionCount + carvingVertexIndex;

			face.uniqueIndices.emplace_back(facePositionCount + carvingVertexIndex);

			carvingVertexIndex += 1;
		}

		facePositionCount += static_cast<uint32>(carvingVertexCount);

		face.indices.resize(previousIndexCount + static_cast<uint32>(carvingVertexCount) + 2);

		std::copy_n(&face.indices[0], previousIndexCount, &face.indices[face.indices.size() - previousIndexCount]);

		uint32 index = 0;
		sint32 reverseCarvingIndex = static_cast<sint32>(carvingVertexCount);

		while (reverseCarvingIndex >= 0) {
			face.indices[connectorEdge.indexA + 1 + index] = carvingIndices[(connectorEdge.indexB + reverseCarvingIndex) % carvingVertexCount];

			index += 1;
			reverseCarvingIndex -= 1;
		}

		IndexedFaceDescriptor descriptor{};
		descriptor.indices = carvingIndices;
		descriptor.normal = face.normal;
		descriptor.material = face.material;
		descriptor.textureFiltering = face.textureFiltering;
		descriptor.lightmap = face.lightmap;

		return createFace(descriptor);
	}
}