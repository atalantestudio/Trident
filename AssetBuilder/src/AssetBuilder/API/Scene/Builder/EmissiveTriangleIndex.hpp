// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	sequence<uint32> SceneBuilder::createEmissiveTriangleIndexBuffer(const BuildScene3D& scene) const {
		uint64 emissiveTriangleCount = 0;
		uint32 lightmapIndex = 0;

		while (lightmapIndex < lightmaps.count()) {
			if (lightmapMaterials[lightmapIndex].emissiveIntensity == 0.0f) {
				lightmapIndex += 1;

				continue;
			}

			const Lightmap& lightmap = lightmaps[lightmapIndex];

			emissiveTriangleCount += lightmap.triangleRange.count;
			lightmapIndex += 1;
		}

		sequence<uint32> emissiveTriangleIndices(emissiveTriangleCount + 1);

		emissiveTriangleCount = 0;
		lightmapIndex = 0;

		while (lightmapIndex < lightmaps.count()) {
			if (lightmapMaterials[lightmapIndex].emissiveIntensity == 0.0f) {
				lightmapIndex += 1;

				continue;
			}

			const Lightmap& lightmap = lightmaps[lightmapIndex];
			const BuildGeometry& geometry = scene.geometries[geometries[lightmap.geometryIndex].filteredIndex];
			const uint32 triangleOffset = geometry.indexRange.offset / 3;

			uint32 triangleIndex = 0;

			while (triangleIndex < lightmap.triangleRange.count) {
				emissiveTriangleIndices[emissiveTriangleCount + triangleIndex] = triangleOffset + lightmap.triangleRange.offset + triangleIndex;

				triangleIndex += 1;
			}

			emissiveTriangleCount += lightmap.triangleRange.count;
			lightmapIndex += 1;
		}

		return emissiveTriangleIndices;
	}
}