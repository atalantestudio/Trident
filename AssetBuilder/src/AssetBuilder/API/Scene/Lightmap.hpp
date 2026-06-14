// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	struct Lightmap {
		vec3<float32> objectScale;
		svec2<float32> pixelScale;
		ref<GeometryAsset> geometryAsset;
		range<uint16, uint16> edgeIndexRange;
		Extent2D offset;
		Extent2D size;
		uint32 coordinateOffset;
	};

	struct LightmapIndex {
		uint32 index;
		uint32 area;
	};
}