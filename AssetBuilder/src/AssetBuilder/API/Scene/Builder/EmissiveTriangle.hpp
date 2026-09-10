// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	sequence<BuildTriangle3D> SceneBuilder::createEmissiveTriangleBuffer(const BuildScene3D& scene) const {
		uint64 emissiveTriangleCount = 0;
		uint32 lightmapIndex = 0;

		while (lightmapIndex < lightmaps.size()) {
			if (lightmapMaterials[lightmapIndex].emissiveIntensity == 0.0f) {
				lightmapIndex += 1;

				continue;
			}

			const Lightmap& lightmap = lightmaps[lightmapIndex];

			emissiveTriangleCount += lightmap.triangleRange.count;
			lightmapIndex += 1;
		}

		sequence<BuildTriangle3D> emissiveTriangles(emissiveTriangleCount + 1);
		uint64 emissiveTriangleOffset = 0;

		lightmapIndex = 0;

		while (lightmapIndex < lightmaps.size()) {
			if (lightmapMaterials[lightmapIndex].emissiveIntensity == 0.0f) {
				lightmapIndex += 1;

				continue;
			}

			const Lightmap& lightmap = lightmaps[lightmapIndex];
			const BuildGeometry& geometry = scene.geometries[geometries[lightmap.geometryIndex].filteredIndex];

			uint32 triangleIndex = 0;

			while (triangleIndex < lightmap.triangleRange.count) {
				// TODO(wip)
				/* emissiveTriangles[emissiveTriangleOffset + lightmap.triangleRange.offset + triangleIndex] = {
					.indexOffset = geometry.indexRange.offset + lightmap.triangleRange.offset * 3,
					.instanceIndex = 0,
					.materialIndex = lightmapIndex,
				}; */

				triangleIndex += 1;
			}

			emissiveTriangleOffset += 1;
			lightmapIndex += 1;
		}

		return emissiveTriangles;
	}
}