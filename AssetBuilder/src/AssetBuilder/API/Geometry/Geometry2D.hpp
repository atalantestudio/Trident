// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/Filesystem/Asset/GeometryAssetFlags.hpp"

namespace Trident {
	struct BuildGeometry2D {
		GeometryAssetFlags flags;
		std::vector<float32> vertexComponentBuffer;
		std::vector<uint32> indexBuffer;
		Geometry2D geometry;
		Hull2D hull;
	};
}