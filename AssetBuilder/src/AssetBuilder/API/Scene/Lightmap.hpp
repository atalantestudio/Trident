// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	struct Lightmap {
		uint32 triangleOffset;
		Extent2D offset;
		Extent2D size;
		uint32 coordinateOffset;
	};

	struct LightmapIndex {
		uint32 index;
		uint32 area;
	};
}