// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Lightmap.hpp"
#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

namespace Trident {
	void StaticSceneBuilder::generateLightmaps(StaticScene3D& scene) const {
		sequence<Lightmap> lightmaps(subsurfaceCount);

		// Sorted by ascending area.
		std::vector<LightmapIndex> lightmapIndices;
		lightmapIndices.reserve(subsurfaceCount);

		// TODO(asset-builder/scene): Restore.
		/* uint32 quadOffset = 0;

		for (const StaticSurface& surface : surfaces) {
			const uint32 triangleOffset = scene.materials[surface.materialIndex].triangleRange.offset + surface.triangleOffset;

			uint32 quadIndex = 0;

			while (quadIndex < surface.subsurfaces.size()) {
				const StaticSubsurface& quad = surface.subsurfaces[quadIndex];

				pool<svec2<float32>, 4> vertices;
				vertices.add({0.0f, 0.0f});
				vertices.add({quad.end.x - quad.start.x, 0.0f});
				vertices.add({0.0f, quad.end.y - quad.start.y});
				vertices.add({quad.end - quad.start});

				Lightmap& lightmap = lightmaps[quadOffset + quadIndex];
				lightmap.triangleOffset = triangleOffset + quadIndex * 2;
				lightmap.offset = {0, 0};
				lightmap.coordinateOffset = triangleOffset * 2 + quadIndex * 4;

				svec2<float32> maxCoordinate = (quad.end - quad.start) / surface.luxelScale;
				maxCoordinate.x = round(maxCoordinate.x * 100.0f) / 100.0f;
				maxCoordinate.y = round(maxCoordinate.y * 100.0f) / 100.0f;

				DEBUG_ASSERT(maxCoordinate.x > 0.0f);

				uint16 coordinateIndex = 0;

				while (coordinateIndex < vertices.count()) {
					svec2<float32>& coordinate = scene.lightmapCoordinates[lightmap.coordinateOffset + coordinateIndex];
					coordinate = vertices[coordinateIndex] / surface.luxelScale;
					coordinate.x = round(coordinate.x * 100.0f) / 100.0f;
					coordinate.y = round(coordinate.y * 100.0f) / 100.0f;

					coordinateIndex += 1;
				}

				// Pad for dilation.
				maxCoordinate.x += 2.0f;
				maxCoordinate.y += 2.0f;

				const svec2<float32> size = abs(maxCoordinate);

				lightmap.size = {static_cast<uint32>(size.x), static_cast<uint32>(size.y)};

				LightmapIndex lightmapIndex{};
				lightmapIndex.index = quadOffset + quadIndex;
				lightmapIndex.area = lightmap.size.width * lightmap.size.height;

				uint32 lightmapInsertionIndex = 0;

				while (lightmapInsertionIndex < lightmapIndices.size()) {
					if (lightmapIndex.area > lightmapIndices[lightmapInsertionIndex].area) {
						break;
					}

					lightmapInsertionIndex += 1;
				}

				lightmapIndices.insert(lightmapIndices.begin() + lightmapInsertionIndex, lightmapIndex);

				quadIndex += 1;
			}

			quadOffset += quadIndex;
		}

		Lightmapper::updateAtlasCoordinates(scene, lightmaps, view<LightmapIndex>(&lightmapIndices[0], lightmapIndices.size()));

		scene.lightmapAtlasImageAssetDescriptor = {
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {Lightmapper::ATLAS_SIZE.width, Lightmapper::ATLAS_SIZE.width, 1},
			.mipLevelCount = 1,
			.layerCount = 1,
			.data = sequence<uint8>(Lightmapper::ATLAS_SIZE.width * Lightmapper::ATLAS_SIZE.height * getByteSize(ImageFormat::R8G8B8A8_UNORM)),
		};
		scene.lightmapAtlasImageAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();

		Lightmapper lightmapper;
		lightmapper.buildLightmapAtlas(scene, scene.lightmapAtlasImageAssetDescriptor); */
	}
}