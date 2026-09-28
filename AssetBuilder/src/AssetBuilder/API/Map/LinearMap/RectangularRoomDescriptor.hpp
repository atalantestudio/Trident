// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/LightmapDescriptor.hpp"
#include "AssetBuilder/API/Material/MaterialReference.hpp"
#include "AssetBuilder/API/TextureFilteringDescriptor.hpp"

namespace Trident {
	struct RectangularRoomDescriptor {
		static constexpr uint32 FLOOR_FACE = 0;
		static constexpr uint32 CEILING_FACE = 1;
		static constexpr uint32 FRONT_WALL_FACE = 2;
		static constexpr uint32 RIGHT_WALL_FACE = 3;
		static constexpr uint32 BACK_WALL_FACE = 4;
		static constexpr uint32 LEFT_WALL_FACE = 5;

		svec3<float32> origin;
		svec3<float32> size;
		MaterialReference material;
		TextureFilteringDescriptor textureFiltering;
		LightmapDescriptor lightmap;
	};
}