// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/LightmapDescriptor.hpp"
#include "AssetBuilder/API/Material/MaterialReference.hpp"
#include "AssetBuilder/API/Transform.hpp"

namespace Trident {
	struct MeshDescriptor {
		uint32 geometryIndex;
		MaterialReference material;
		BuildTransform3D transform;
		LightmapDescriptor lightmap;

		// TODO(asset-builder/mesh): Artificial scale multiplier in case the geometry is too small.
		vec2<float32> lightmapScale = {1.0f, 1.0f};
	};
}