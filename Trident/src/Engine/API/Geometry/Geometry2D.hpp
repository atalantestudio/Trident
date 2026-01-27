// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2DType.hpp"

namespace Trident {
	class Geometry2D {
		public:
			uint32 getIndexCount() const {
				return static_cast<uint32>(indices.count());
			}

			uint32 getVertexCount() const {
				return static_cast<uint32>(vertices.count());
			}

			virtual float32 getPenetrationBias() const = 0;

			virtual vec2<float32> calculateCentroid() const = 0;

			virtual float32 calculateMass(float32 density, const vec2<float32>& scale) const = 0;

			virtual float32 calculateRadius() const = 0;

			// Returns the farthest point on the geometry along `d`. `d` does not need to be normalized.
			virtual vec2<float32> support(const vec2<float32>& d) const = 0;

			// Returns the index of the farthest point on the geometry along `d`. `d` does not need to be normalized.
			// NOTE: Some geometries like CircleGeometry2D return more precise support points. In these cases, `support` must be used.
			virtual uint32 supportIndex(const vec2<float32>& d) const = 0;

		public:
			Geometry2DType type;

			// The index sequence must be readable with both TRIANGLE_LIST and LINE_STRIP topologies.
			sequence<uint32> indices;

			sequence<vec2<float32>> vertices;

			// Average of all vertices.
			vec2<float32> centroid;

			// Distance between the center of mass and the farthest vertex.
			float32 radius;
	};
}