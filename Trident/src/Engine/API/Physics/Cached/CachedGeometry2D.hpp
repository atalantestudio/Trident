// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2DType.hpp"

namespace Trident {
	struct CachedGeometry2D {
		svec2<float32> centroid;
		float32 radius;
		uint32 vertexOffset;
		uint32 vertexCount;
		uint32 indexOffset;
		uint16 triangleListIndexCount;
		uint16 lineStripIndexCount;
		Hull2DType type;
	};
}