// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/LightmapDescriptor.hpp"
#include "AssetBuilder/API/Material/MaterialReference.hpp"

namespace Trident {
	struct RectangularRoomDescriptor {
		svec3<float32> origin;
		svec3<float32> size;
		MaterialReference materialReference;
		LightmapDescriptor lightmap;
	};
}