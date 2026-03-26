// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace Trident {
	class CachedGeometry2D {
		public:
			CachedGeometry2D() = default;

			explicit CachedGeometry2D(ref<Geometry2D> geometry, uint32 vertexOffset, uint32 indexOffset) :
				centroid({geometry->centroid[0], geometry->centroid[1]}),
				radius(geometry->radius),
				vertexOffset(vertexOffset),
				indexOffset(indexOffset),
				indexCount(static_cast<uint16>(geometry->getIndexCount())),
				type(geometry->type)
			{}

		public:
			svec2<float32> centroid;
			float32 radius;
			uint32 vertexOffset;
			uint32 indexOffset;
			uint16 indexCount;
			Geometry2DType type;
	};
}