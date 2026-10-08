// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	BuildGeometry3D inflateFBXStreams(
		view<vec3<float32>> positions,
		view<vec3<float32>> normals,
		view<vec2<float32>> textureCoordinates,
		view<uint32> positionIndices,
		view<uint32> normalIndices,
		view<uint32> textureCoordinateIndices
	) {
		ENGINE_ASSERT(positionIndices.count() == normalIndices.count() && positionIndices.count() == textureCoordinateIndices.count(), "The index streams must have the same size.");

		const uint64 indexCount = positionIndices.count();

		BuildGeometry3D geometry{};
		geometry.flags = GeometryAssetFlags::IS_3D;
		geometry.indices = sequence<uint32>(indexCount);

		std::unordered_map<GeometryIndex3D, uint32> keys;
		sequence<GeometryIndex3D> vertexKeys(indexCount);

		uint32 vertexCount = 0;
		uint64 indexOffset = 0;

		while (indexOffset < indexCount) {
			const GeometryIndex3D key = {
				positionIndices[indexOffset],
				normalIndices[indexOffset],
				textureCoordinateIndices[indexOffset],
			};

			auto iterator = keys.find(key);

			if (iterator != keys.end()) {
				geometry.indices[indexOffset] = iterator->second;

				indexOffset += 1;

				continue;
			}

			keys.insert({key, vertexCount});

			vertexKeys[vertexCount] = key;
			geometry.indices[indexOffset] = vertexCount;

			vertexCount += 1;
			indexOffset += 1;
		}

		geometry.vertices = sequence<BuildVertex3D>(vertexCount);

		uint64 vertexIndex = 0;

		while (vertexIndex < vertexCount) {
			const GeometryIndex3D key = vertexKeys[vertexIndex];

			BuildVertex3D& vertex = geometry.vertices[vertexIndex];
			vertex.position = positions[key.positionIndex];
			vertex.normal = normals[key.normalIndex];
			vertex.textureCoordinate = textureCoordinates[key.textureCoordinateIndex];

			vertexIndex += 1;
		}

		return geometry;
	}
}