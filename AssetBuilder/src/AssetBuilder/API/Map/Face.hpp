// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/LightmapDescriptor.hpp"
#include "AssetBuilder/API/Material/MaterialReference.hpp"

namespace Trident {
	struct AABB2D {
		svec2<float32> min;
		svec2<float32> max;
	};

	struct FaceDescriptor {
		view<svec3<float32>> vertices;
		svec3<float32> normal;
		MaterialReference materialReference;
		LightmapDescriptor lightmap;
	};

	struct Face {
		uint32 vertexCount;

		// TODO(asset-builder/map/static-geometry)
		// Remove the unused range's count (it's currently equal to the vertex count).
		range<uint32, uint32> indexRange;

		svec3<float32> normal;
		uint32 geometryIndex;
		MaterialReference materialReference;
		LightmapDescriptor lightmap;
		AABB2D boundingBox;
	};
}