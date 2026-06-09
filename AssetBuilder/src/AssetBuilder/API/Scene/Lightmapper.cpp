// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/GUI/LayoutDirection.hpp"

#include "AssetBuilder/API/Scene/Lightmapper.hpp"

namespace Trident {
	static constexpr Extent2D ATLAS_SIZE = {2048, 2048};

	struct LightmapNode {
		LightmapNode* children[2]{};

		/// Direction in which to split the node.
		LayoutDirection splitDirection;

		/// Size along the split direction.
		uint32 size;

		uint32 lightmapIndex = max<uint32>();

		~LightmapNode() {
			delete children[0];
			delete children[1];
		}

		bool leaf() const {
			return children[0] == nullptr && children[1] == nullptr;
		}

		bool canContain(Extent2D extent) const {
			return size >= extent[splitDirection];
		}

		Extent2D getExtent() const {
			Extent2D extent = ATLAS_SIZE;
			extent[splitDirection] = size;

			return extent;
		}
	};

	static bool fitsInside(Extent2D container, Extent2D contained) {
		return container.width >= contained.width && container.height >= contained.height;
	}

	static LightmapNode* insertNode(const Lightmap& lightmap, LightmapNode* node) {
		if (!node->leaf()) {
			LightmapNode* newNode = insertNode(lightmap, node->children[0]);

			if (newNode != nullptr) {
				return newNode;
			}

			return insertNode(lightmap, node->children[1]);
		}

		DEBUG_ASSERT(node->lightmapIndex == max<uint32>());

		const Extent2D nodeSize = node->getExtent();
		const Extent2D lightmapSize = {
			static_cast<uint32>(ceil(lightmap.size.x)),
			static_cast<uint32>(ceil(lightmap.size.y)),
		};

		if (!node->canContain(lightmapSize)) {
			return nullptr;
		}

		if (node->size == lightmapSize[node->splitDirection]) {
			return node;
		}

		node->children[0] = new LightmapNode();
		node->children[1] = new LightmapNode();

		const Extent2D remainingSize = {
			nodeSize.width - lightmapSize.width,
			nodeSize.height - lightmapSize.height,
		};

		if (remainingSize.width > remainingSize.height) {
			node->children[0]->splitDirection = LayoutDirection::HORIZONTAL;
			node->children[0]->size = lightmapSize.width;

			node->children[1]->splitDirection = LayoutDirection::HORIZONTAL;
			node->children[1]->size = ATLAS_SIZE[node->children[1]->splitDirection] - lightmapSize.width;
		}
		else {
			node->children[0]->splitDirection = LayoutDirection::VERTICAL;
			node->children[0]->size = lightmapSize.height;

			node->children[1]->splitDirection = LayoutDirection::VERTICAL;
			node->children[1]->size = ATLAS_SIZE[node->children[1]->splitDirection] - lightmapSize.height;
		}

		return insertNode(lightmap, node->children[0]);
	}

	void Lightmapper::updateAtlasCoordinates(view<Lightmap> lightmaps, sequence<StaticTriangle3D>& triangles) {
		LightmapNode rootNode{};
		rootNode.splitDirection = LayoutDirection::HORIZONTAL;
		rootNode.size = ATLAS_SIZE[rootNode.splitDirection];

		uint32 lightmapIndex = 0;

		while (lightmapIndex < lightmaps.count()) {
			LightmapNode* node = insertNode(lightmaps[lightmapIndex], &rootNode);

			lightmapIndex += 1;
		}

		BREAKPOINT();
	}
}