// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	uint32 MapBuilder::loadGeometry(ref<Geometry3DAsset> asset) {
		uint32 geometryIndex = 0;

		while (geometryIndex < geometries.size()) {
			const IntermediateGeometry& geometry = geometries[geometryIndex];

			if (asset->identifier == geometry.assetIdentifier) {
				break;
			}

			geometryIndex += 1;
		}

		if (geometryIndex >= geometries.size()) {
			IntermediateGeometry geometry{};
			geometry.assetIdentifier = asset->identifier;
			geometry.sharedInstanceCount = 0;
			geometry.vertices = sequence<BuildVertex3D>(asset->vertices.count());
			geometry.indices = sequence<uint32>(asset->indices.count());

			uint32 vertexIndex = 0;

			while (vertexIndex < geometry.vertices.count()) {
				const Vertex3D& packedVertex = asset->vertices[vertexIndex];

				BuildVertex3D& vertex = geometry.vertices[vertexIndex];
				vertex.position = packedVertex.position;
				vertex.normal = asset->vertexNormals[vertexIndex];
				vertex.textureCoordinate = unpack<VertexInputAttributeFormat::R16G16_FLOAT, svec2<float32>>(packedVertex.textureCoordinate);

				vertexIndex += 1;
			}

			std::copy(asset->indices.begin(), asset->indices.end(), &geometry.indices[0]);

			geometries.emplace_back(geometry);
		}

		return geometryIndex;
	}
}