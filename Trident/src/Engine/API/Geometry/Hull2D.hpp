// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Hull2DType.hpp"

namespace Trident {
	/// A convex hull wrapping a 2D geometry.
	struct Hull2D {
		public:
			/// Returns the farthest point on the hull along `d`.
			inline vec2<float32> vertexSupport(vec2<float32> d) const {
				// TODO(physics/2d/hull/support): Replace with LUT.
				switch (type) {
					case Hull2DType::CIRCLE: {
						return normalized(d) * radius;
					}
					case Hull2DType::POLYGON: {
						return vertices[indexSupport(d)];
					}
					default: {
						DEBUG_ASSERT(false);

						return {0.0f, 0.0f};
					}
				}
			}

			// Some geometries like CircleGeometry2D return more precise support points. In these cases `support` must be used.
			/// Returns the index of the farthest point on the hull along `d`.
			inline uint32 indexSupport(vec2<float32> d) const {
				DEBUG_ASSERT(type == Hull2DType::POLYGON);

				float32 maxValue = dot(vertices[0], d);
				uint32 maxVertexIndex = 0;
				uint32 vertexIndex = 1;

				while (vertexIndex < vertices.count()) {
					const float32 value = dot(vertices[vertexIndex], d);

					if (value > maxValue) {
						maxValue = value;
						maxVertexIndex = vertexIndex;
					}

					vertexIndex += 1;
				}

				return maxVertexIndex;
			}

		public:
			Hull2DType type;

			/// Average of all vertices.
			vec2<float32> centroid;

			/// The distance between the centroid and the farthest vertex.
			float32 radius;

			// Vertex positions on the parent geometry.
			view<vec2<float32>> vertices;

			// TODO(physics/2d/hull): Use 16-bit indices.
			range<uint32, uint32> lineIndexRange;

			// TODO(physics/2d/hull): Replace with a view inside a contiguous buffer.
			sequence<Edge2D> edges;
	};
}