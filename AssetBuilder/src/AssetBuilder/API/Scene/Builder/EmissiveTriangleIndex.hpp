// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	sequence<uint32> SceneBuilder::createEmissiveTriangleIndexBuffer(const BuildScene3D& scene) const {
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

		sequence<uint32> emissiveTriangleIndices(emissiveTriangleCount + 1);

		emissiveTriangleCount = 0;
		lightmapIndex = 0;

		while (lightmapIndex < lightmaps.size()) {
			if (lightmapMaterials[lightmapIndex].emissiveIntensity == 0.0f) {
				lightmapIndex += 1;

				continue;
			}

			const Lightmap& lightmap = lightmaps[lightmapIndex];

			uint32 triangleIndex = 0;

			while (triangleIndex < lightmap.triangleRange.count) {
				emissiveTriangleIndices[emissiveTriangleCount + triangleIndex] = lightmap.triangleRange.offset + triangleIndex;

				triangleIndex += 1;
			}

			emissiveTriangleCount += lightmap.triangleRange.count;
			lightmapIndex += 1;
		}

		return emissiveTriangleIndices;
	}
}