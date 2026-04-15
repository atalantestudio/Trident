// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Hull2DType.hpp"

namespace Trident {
	/// A convex hull wrapping a 2D geometry.
	struct Hull2D {
		private:
			// Point hulls need to have 1 vertex for GJK to work.
			inline static const sequence<svec2<float32>> pointHullVertices = {{0.0f, 0.0f}};

		public:
			/// Creates and returns a point hull.
			/// When coupled with a rigid body, point hulls can represent the position of a 2D point (e.g. the pointer's position).
			inline static Hull2D point() {
				return {
					.type = Hull2DType::POINT,
					.centroid = {0.0_m, 0.0_m},
					.radius = 0.0_m,
					.vertices = pointHullVertices,
					.lineIndexRange = {0, 0},
					.edges = {},
				};
			}

		public:
			inline float32 getPenetrationBias() const {
				static constexpr float32 PENETRATION_BIASES[static_cast<uint32>(Hull2DType::count)] = {
					0.0f,
					0.01f,
					0.2f,
				};

				DEBUG_ASSERT(type < Hull2DType::count);

				return PENETRATION_BIASES[static_cast<uint32>(type)];
			}

			/// Returns the farthest point on the hull along `d`.
			inline vec2<float32> support(vec2<float32> d) const {
				// TODO: Replace with LUT.
				switch (type) {
					case Hull2DType::CIRCLE: {
						return normalized(d) * radius;
					}
					case Hull2DType::POINT: {
						return {0.0_m, 0.0_m};
					}
					case Hull2DType::POLYGON: {
						const svec2<float32> vertex = vertices[supportIndex(d)];

						return {vertex[0], vertex[1]};
					}
					default: {
						DEBUG_ASSERT(false);

						return {0.0_m, 0.0_m};
					}
				}
			}

			// NOTE: Some geometries like CircleGeometry2D return more precise support points. In these cases, `support` must be used.
			/// Returns the index of the farthest point on the hull along `d`.
			inline uint32 supportIndex(vec2<float32> d) const {
				// TODO: Replace with LUT.
				switch (type) {
					case Hull2DType::CIRCLE: {
						ENGINE_ASSERT(false, "Use Geometry2D::support to get a support point on a circle geometry.");

						return 0;
					}
					case Hull2DType::POINT: {
						return 0;
					}
					case Hull2DType::POLYGON: {
						float32 maxValue = dot(vec2<float32>(vertices[0]), d);
						uint32 maxVertexIndex = 0;
						uint32 vertexIndex = 1;

						while (vertexIndex < vertices.count()) {
							const float32 value = dot(vec2<float32>(vertices[vertexIndex]), d);

							if (value > maxValue) {
								maxValue = value;
								maxVertexIndex = vertexIndex;
							}

							vertexIndex += 1;
						}

						return maxVertexIndex;
					}
					default: {
						DEBUG_ASSERT(false);

						return 0;
					}
				}
			}

		public:
			Hull2DType type;

			/// Average of all vertices.
			svec2<float32> centroid;

			/// The distance between the centroid and the farthest vertex.
			float32 radius;

			// Vertex positions on the parent geometry.
			view<svec2<float32>> vertices;

			// TODO: Use 16-bit indices.
			range<uint32, uint32> lineIndexRange;

			// TODO(physics/hull-edges): Replace with a view inside a contiguous buffer.
			sequence<Edge2D> edges;
	};
}