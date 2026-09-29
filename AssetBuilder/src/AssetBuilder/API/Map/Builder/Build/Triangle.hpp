// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	sequence<BuildTriangle3D> MapBuilder::createTriangleBuffer(const BuildScene3D& scene) const {
		uint64 triangleCount = 0;
		uint32 lightmapIndex = 0;

		while (lightmapIndex < lightmaps.count()) {
			const Lightmap& lightmap = lightmaps[lightmapIndex];

			triangleCount += lightmap.triangleRange.count;
			lightmapIndex += 1;
		}

		sequence<BuildTriangle3D> triangles(triangleCount);
		uint64 triangleOffset = 0;

		lightmapIndex = 0;

		while (lightmapIndex < lightmaps.count()) {
			const Lightmap& lightmap = lightmaps[lightmapIndex];

			uint32 triangleIndex = 0;

			while (triangleIndex < lightmap.triangleRange.count) {
				const uint32 indexOffset = (lightmap.triangleRange.offset + triangleIndex) * 3;

				triangles[triangleOffset + triangleIndex] = {
					.index = lightmap.triangleRange.offset + triangleIndex,
					.lightmapCoordinateIndexA = scene.indices[indexOffset + 0],
					.lightmapCoordinateIndexB = scene.indices[indexOffset + 1],
					.lightmapCoordinateIndexC = scene.indices[indexOffset + 2],
					.materialIndex = lightmap.materialIndex,
				};

				triangleIndex += 1;
			}

			triangleOffset += lightmap.triangleRange.count;
			lightmapIndex += 1;
		}

		return triangles;
	}
}