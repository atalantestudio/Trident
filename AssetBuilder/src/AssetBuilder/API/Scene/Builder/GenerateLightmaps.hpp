// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmap.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"
#include "AssetBuilder/API/Scene/Builder/StaticSceneBuilder.hpp"

namespace Trident {
	// See https://blackpawn.com/texts/lightmaps.
	LightmapNode* StaticSceneBuilder::insertLightmapNode(Lightmap& lightmap, LightmapNode* node) {
		if (!node->leaf()) {
			LightmapNode* newNode = insertLightmapNode(lightmap, node->children[0]);

			if (newNode != nullptr) {
				return newNode;
			}

			return insertLightmapNode(lightmap, node->children[1]);
		}

		if (node->assigned) {
			return nullptr;
		}

		if (
			node->rectangle.width < lightmap.size.width ||
			node->rectangle.height < lightmap.size.height
		) {
			return nullptr;
		}

		if (
			node->rectangle.width == lightmap.size.width &&
			node->rectangle.height == lightmap.size.height
		) {
			return node;
		}

		node->children[0] = new LightmapNode();
		node->children[0]->rectangle = node->rectangle;

		node->children[1] = new LightmapNode();
		node->children[1]->rectangle = node->rectangle;

		const Extent2D remainingSize = {
			node->rectangle.width - lightmap.size.width,
			node->rectangle.height - lightmap.size.height,
		};

		// Split towards the axis with the largest available space.
		if (remainingSize.width > remainingSize.height) {
			node->children[0]->rectangle.width = lightmap.size.width;

			node->children[1]->rectangle.x += lightmap.size.width;
			node->children[1]->rectangle.width -= lightmap.size.width;
		}
		else {
			node->children[0]->rectangle.height = lightmap.size.height;

			node->children[1]->rectangle.y += lightmap.size.height;
			node->children[1]->rectangle.height -= lightmap.size.height;
		}

		return insertLightmapNode(lightmap, node->children[0]);
	}

	void StaticSceneBuilder::generateLightmaps(StaticScene3D& scene) {
		createLightmaps(scene);
		updateLightmapCoordinates(scene);

		scene.lightmapAtlasImageAssetDescriptor = {
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {LIGHTMAP_ATLAS_EXTENT.width, LIGHTMAP_ATLAS_EXTENT.height, 1},
			.mipLevelCount = 1,
			.layerCount = 1,
			.data = sequence<uint8>(LIGHTMAP_ATLAS_EXTENT.width * LIGHTMAP_ATLAS_EXTENT.height * getByteSize(ImageFormat::R8G8B8A8_UNORM)),
		};
		scene.lightmapAtlasImageAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();

		lightmapper.setOptions({
			.atlasFormat = scene.lightmapAtlasImageAssetDescriptor.format,
			.atlasExtent = LIGHTMAP_ATLAS_EXTENT,
			.skyColor = skyColor,
			.sunDirection = sunDirection,
			.sunColor = sunColor,
			.sunIntensity = sunIntensity,
		});
		lightmapper.buildLightmapAtlas(scene, scene.lightmapAtlasImageAssetDescriptor);
	}

	void StaticSceneBuilder::createLightmaps(StaticScene3D& scene) {
		lightmapCoordinates = sequence<svec2<float32>>(subsurfaceTextureCoordinateCount);

		lightmapLookups.reserve(subsurfaceCount);

		uint32 indexOffset;
		uint32 lightmapIndex = 0;
		uint32 lightmapCoordinateOffset = 0;

		for (const StaticSurface& surface : surfaces) {
			indexOffset = scene.materials[surface.materialIndex].indexRange.offset + surface.indexOffset;

			for (const StaticSubsurface& subsurface : surface.subsurfaces) {
				const uint64 textureCoordinateCount = subsurface.textureCoordinates.count();

				Lightmap& lightmap = lightmaps[lightmapIndex];
				lightmap.offset = {0, 0};
				lightmap.indexOffset = indexOffset;
				lightmap.coordinateOffset = lightmapCoordinateOffset;
				lightmap.subsurface = &subsurface;

				uint32 textureCoordinateIndex = 0;

				while (textureCoordinateIndex < textureCoordinateCount) {
					lightmapCoordinates[lightmap.coordinateOffset + textureCoordinateIndex] = (subsurface.textureCoordinates[textureCoordinateIndex] - surface.minCoordinate) * subsurface.scale / surface.luxelScale;

					textureCoordinateIndex += 1;
				}

				// TODO(lyah/scalar)
				const vec2<float32> size = round<2, 2, float32>(vec2<float32>(subsurface.scale / surface.luxelScale));

				lightmap.size = {static_cast<uint32>(size[0]), static_cast<uint32>(size[1])};

				DEBUG_ASSERT(lightmap.size.width > 0 && lightmap.size.height > 0);

				// Pad for dilation and bicubic filtering.
				lightmap.size.width += 4;
				lightmap.size.height += 4;

				const LightmapLookup lightmapLookup = {
					.index = lightmapIndex,
					.area = lightmap.size.width * lightmap.size.height,
				};

				uint32 lightmapInsertionIndex = 0;

				while (lightmapInsertionIndex < lightmapLookups.size()) {
					if (lightmapLookup.area > lightmapLookups[lightmapInsertionIndex].area) {
						break;
					}

					lightmapInsertionIndex += 1;
				}

				lightmapLookups.insert(lightmapLookups.begin() + lightmapInsertionIndex, lightmapLookup);

				indexOffset += static_cast<uint32>(subsurface.vertexIndices.count());
				lightmapIndex += 1;
				lightmapCoordinateOffset += static_cast<uint32>(textureCoordinateCount);
			}
		}
	}

	void StaticSceneBuilder::updateLightmapCoordinates(StaticScene3D& scene) {
		LightmapNode rootNode{};
		rootNode.rectangle = {0, 0, LIGHTMAP_ATLAS_EXTENT.width, LIGHTMAP_ATLAS_EXTENT.height};

		for (const LightmapLookup lightmapLookup : lightmapLookups) {
			Lightmap& lightmap = lightmaps[lightmapLookup.index];

			LightmapNode* node = insertLightmapNode(lightmap, &rootNode);

			DEBUG_ASSERT(node != nullptr);

			node->assigned = true;

			lightmap.offset.width = node->rectangle.x;
			lightmap.offset.height = node->rectangle.y;

			uint32 coordinateIndex = 0;

			while (coordinateIndex < lightmap.subsurface->textureCoordinates.count()) {
				// TODO(lyah/scalar)
				svec2<float32>& coordinate = lightmapCoordinates[lightmap.coordinateOffset + coordinateIndex];
				coordinate.x += lightmap.offset.width;
				coordinate.y += lightmap.offset.height;
				coordinate /= svec2<float32>{static_cast<float32>(LIGHTMAP_ATLAS_EXTENT.width), static_cast<float32>(LIGHTMAP_ATLAS_EXTENT.height)};

				coordinateIndex += 1;
			}

			const view<uint32> textureCoordinateIndices = lightmap.subsurface->textureCoordinateIndices;

			uint32 indexOffset = 0;

			while (indexOffset < lightmap.subsurface->vertexIndices.count()) {
				StaticVertex3D& index = scene.indices[lightmap.indexOffset + indexOffset];
				index.lightmapCoordinate = lightmapCoordinates[lightmap.coordinateOffset + textureCoordinateIndices[indexOffset]];

				indexOffset += 1;
			}
		}
	}
}