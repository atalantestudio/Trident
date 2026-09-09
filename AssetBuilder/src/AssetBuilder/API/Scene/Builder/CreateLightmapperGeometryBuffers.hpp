// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	void SceneBuilder::createLightmapperGeometryBuffers(const BuildScene3D& scene, sequence<BuildTriangle3D>& triangles, sequence<uint32>& emissiveTriangleIndices, sequence<LightmapMaterial>& materials) {
		triangles = sequence<BuildTriangle3D>(scene.staticGeometry.unwrappedTriangleCount);

		materials = sequence<LightmapMaterial>(lightmapMaterials.size());

		std::copy(lightmapMaterials.begin(), lightmapMaterials.end(), &materials[0]);

		std::vector<uint32> emissiveTriangleIndexData;

		uint32 lightmapIndex = 0;
		uint64 vertexOffset = 0;

		while (lightmapIndex < lightmaps.size()) {
			const Lightmap& lightmap = lightmaps[lightmapIndex];
			const uint32 geometryTriangleOffset = scene.geometries[lightmap.geometryIndex].indexRange.offset / 3;

			uint32 triangleIndex = 0;

			while (triangleIndex < lightmap.triangleRange.count) {
				triangles[geometryTriangleOffset + lightmap.triangleRange.offset + triangleIndex] = {
					.geometryOffset = lightmap.triangleRange.offset,
					.materialIndex = lightmapIndex,
				};

				triangleIndex += 1;
			}

			if (lightmapMaterials[lightmapIndex].emissiveIntensity > 0.0f) {
				uint32 triangleIndex = 0;

				while (triangleIndex < lightmap.triangleRange.count) {
					emissiveTriangleIndexData.emplace_back(geometryTriangleOffset + lightmap.triangleRange.offset + triangleIndex);

					triangleIndex += 1;
				}
			}

			lightmapIndex += 1;
		}

		emissiveTriangleIndices = sequence<uint32>(emissiveTriangleIndexData.size());

		std::copy(emissiveTriangleIndexData.begin(), emissiveTriangleIndexData.end(), &emissiveTriangleIndices[0]);
	}
}