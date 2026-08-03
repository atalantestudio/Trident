// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmap.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"
#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	// See https://blackpawn.com/texts/lightmaps.
	LightmapNode* SceneBuilder::insertLightmapNode(Lightmap& lightmap, LightmapNode* node) {
		if (!node->leaf()) {
			LightmapNode* newNode = insertLightmapNode(lightmap, node->children[0]);

			if (newNode != nullptr) {
				return newNode;
			}

			return insertLightmapNode(lightmap, node->children[1]);
		}

		if (node->index < max<uint32>()) {
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
			node->children[1]->rectangle.width = remainingSize.width;
		}
		else {
			node->children[0]->rectangle.height = lightmap.size.height;

			node->children[1]->rectangle.y += lightmap.size.height;
			node->children[1]->rectangle.height = remainingSize.height;
		}

		return insertLightmapNode(lightmap, node->children[0]);
	}

	void SceneBuilder::createLightmaps(BuildScene3D& scene) {
		uint32 lightmapCoordinateOffset = 0;
		uint32 lightmapIndex = 0;

		while (lightmapIndex < lightmaps.size()) {
			Lightmap& lightmap = lightmaps[lightmapIndex];
			lightmap.coordinateOffset = lightmapCoordinateOffset;

			const IntermediateGeometry& geometry = geometries[lightmap.geometryIndex];
			const view<GeometryIndex3D> vertices(&geometry.vertices[lightmap.vertexRange.offset], lightmap.vertexRange.count);

			uint32 coordinateIndex = 0;

			while (coordinateIndex < lightmap.vertexRange.count) {
				const uint32 textureCoordinateIndex = vertices[coordinateIndex].textureCoordinateIndex;
				const svec2<float32> textureCoordinate = geometry.textureCoordinates[textureCoordinateIndex];

				scene.staticGeometry.lightmapCoordinates[lightmap.coordinateOffset + coordinateIndex] = (textureCoordinate - lightmap.minCoordinate) * lightmap.scale / lightmap.luxelScale;

				coordinateIndex += 1;
			}

			// TODO(lyah/scalar)
			vec2<float32> size = lightmap.scale / lightmap.luxelScale;
			size = {ceil(size[0] / 4.0f) * 4.0f, ceil(size[1] / 4.0f) * 4.0f};

			lightmap.size = {static_cast<uint32>(size[0]), static_cast<uint32>(size[1])};

			DEBUG_ASSERT(lightmap.size.width > 0 && lightmap.size.height > 0);

			// Dilation needs 1px padding on all sides, bicubic filtering needs 2.
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

			lightmapCoordinateOffset += static_cast<uint32>(vertices.count());
			lightmapIndex += 1;
		}
	}

	void SceneBuilder::updateLightmapCoordinates(BuildStaticSceneGeometry3D& staticGeometry) {
		LightmapNode rootNode{};
		rootNode.rectangle = {0, 0, LIGHTMAP_ATLAS_EXTENT.width, LIGHTMAP_ATLAS_EXTENT.height};

		for (const LightmapLookup lightmapLookup : lightmapLookups) {
			Lightmap& lightmap = lightmaps[lightmapLookup.index];

			LightmapNode* node = insertLightmapNode(lightmap, &rootNode);

			ENGINE_ASSERT(node != nullptr, "There isn't enough remaining space in the lightmap atlas to insert the node.");

			node->index = lightmapLookup.index;

			uint32 coordinateIndex = 0;

			while (coordinateIndex < lightmap.vertexRange.count) {
				// TODO(lyah/scalar)
				svec2<float32>& coordinate = staticGeometry.lightmapCoordinates[lightmap.coordinateOffset + coordinateIndex];
				coordinate.x += node->rectangle.x;
				coordinate.y += node->rectangle.y;
				coordinate /= svec2<float32>{static_cast<float32>(LIGHTMAP_ATLAS_EXTENT.width), static_cast<float32>(LIGHTMAP_ATLAS_EXTENT.height)};

				coordinateIndex += 1;
			}
		}
	}
}