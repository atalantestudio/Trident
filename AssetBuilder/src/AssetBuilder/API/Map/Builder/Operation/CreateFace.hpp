// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	Face& MapBuilder::createFace(const FaceDescriptor& descriptor) {
		ENGINE_ASSERT(descriptor.vertices.count() >= 3, "Not enough vertices.");

		const uint32 vertexCount = static_cast<uint32>(descriptor.vertices.count());

		sequence<uint32> indices(vertexCount);
		uint32 vertexIndex = 0;

		while (vertexIndex < vertexCount) {
			const svec3<float32> vertex = descriptor.vertices[vertexIndex];

			facePositions[facePositionCount + vertexIndex] = vertex;
			indices[vertexIndex] = facePositionCount + vertexIndex;

			vertexIndex += 1;
		}

		facePositionCount += vertexCount;

		IndexedFaceDescriptor indexedDescriptor{};
		indexedDescriptor.indices = indices;
		indexedDescriptor.normal = descriptor.normal;
		indexedDescriptor.material = descriptor.material;
		indexedDescriptor.textureFiltering = descriptor.textureFiltering;
		indexedDescriptor.lightmap = descriptor.lightmap;

		return createFace(indexedDescriptor);
	}

	Face& MapBuilder::createFace(const IndexedFaceDescriptor& descriptor) {
		ENGINE_ASSERT(descriptor.indices.count() >= 3, "Not enough indices.");

		Face face{};
		face.indices.resize(descriptor.indices.count());
		face.normal = descriptor.normal;
		face.geometryIndex = max<uint32>();
		face.material = descriptor.material;
		face.textureFiltering = descriptor.textureFiltering;
		face.boundingBox.min = {max<float32>(), max<float32>()};
		face.boundingBox.max = {min<float32>(), min<float32>()};
		face.lightmap = descriptor.lightmap;

		std::copy(descriptor.indices.begin(), descriptor.indices.end(), &face.indices[0]);

		faces[staticGeometryFaceCount] = face;

		staticGeometryFaceCount += 1;

		return faces[staticGeometryFaceCount - 1];
	}
}