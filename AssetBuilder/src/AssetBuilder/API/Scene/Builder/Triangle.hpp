// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	sequence<BuildTriangle3D> SceneBuilder::createTriangleBuffer(const BuildScene3D& scene) const {
		uint64 triangleCount = 0;
		uint32 lightmapIndex = 0;

		while (lightmapIndex < lightmaps.size()) {
			const Lightmap& lightmap = lightmaps[lightmapIndex];

			triangleCount += lightmap.triangleRange.count;
			lightmapIndex += 1;
		}

		sequence<BuildTriangle3D> triangles(triangleCount);
		uint64 triangleOffset = 0;

		lightmapIndex = 0;

		while (lightmapIndex < lightmaps.size()) {
			const Lightmap& lightmap = lightmaps[lightmapIndex];
			const BuildGeometry& geometry = scene.geometries[geometries[lightmap.geometryIndex].filteredIndex];

			uint32 triangleIndex = 0;

			while (triangleIndex < lightmap.triangleRange.count) {
				const uint32 indexOffset = triangleIndex * 3;

				triangles[triangleOffset + triangleIndex] = {
					.index = geometry.indexRange.offset / 3 + lightmap.triangleRange.offset + triangleIndex,
					.lightmapCoordinateIndexA = lightmap.coordinateOffset + scene.indices[indexOffset + 0],
					.lightmapCoordinateIndexB = lightmap.coordinateOffset + scene.indices[indexOffset + 1],
					.lightmapCoordinateIndexC = lightmap.coordinateOffset + scene.indices[indexOffset + 2],
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