// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	struct StaticSubsurface;

	struct Lightmap {
		Extent2D offset;
		Extent2D size;
		const StaticSubsurface* subsurface;
		range<uint32, uint32> triangleRange;
		range<uint32, uint32> coordinateRange;
	};

	struct LightmapLookup {
		uint32 index;
		uint32 area;
	};

	struct Rectangle {
		uint32 x;
		uint32 y;
		uint32 width;
		uint32 height;
	};

	struct LightmapNode {
		LightmapNode* children[2]{};
		Rectangle rectangle;
		bool assigned = false;

		~LightmapNode() {
			delete children[0];
			delete children[1];
		}

		bool leaf() const {
			return children[0] == nullptr && children[1] == nullptr;
		}
	};
}