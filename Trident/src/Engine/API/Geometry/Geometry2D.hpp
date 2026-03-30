// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Geometry2DType.hpp"
#include "Engine/API/Physics/Cached/CachedGeometry2D.hpp"

namespace Trident {
	class Geometry2D {
		public:
			static Geometry2D fromCached(const CachedGeometry2D& cachedGeometry, view<float32> vertexBuffer, view<uint16> indexBuffer) {
				Geometry2D geometry = {
					.type = cachedGeometry.type,
					.centroid = {cachedGeometry.centroid[0], cachedGeometry.centroid[1]},
					.radius = cachedGeometry.radius,
					.vertexOffset = cachedGeometry.vertexOffset,
					.vertices = view<svec2<float32>>(pointer_cast<const svec2<float32>*>(&vertexBuffer[cachedGeometry.vertexOffset * 2]), cachedGeometry.vertexCount),
					.indexOffset = cachedGeometry.indexOffset,
					.triangleListIndices = view<uint16>(&indexBuffer[cachedGeometry.indexOffset], cachedGeometry.triangleListIndexCount),
					.triangleListIndexCount = cachedGeometry.triangleListIndexCount,
					.lineStripIndexCount = cachedGeometry.lineStripIndexCount,
					.edges = sequence<Edge>(cachedGeometry.triangleListIndexCount / 3 * 2),
				};

				uint32 edgeIndex = 0;

				while (edgeIndex < geometry.edges.count()) {
					geometry.edges[edgeIndex] = {edgeIndex, (edgeIndex + 1) % geometry.edges.count()};

					edgeIndex += 1;
				}

				return geometry;
			}

			static Geometry2D pointAtOrigin() {
				return {
					.type = Geometry2DType::POINT,
					.centroid = {0.0_m, 0.0_m},
					.radius = 0.0_m,
					.vertexOffset = 0,
					.indexOffset = 0,
					.triangleListIndexCount = 0,
					.lineStripIndexCount = 0,
				};
			}

		public:
			Geometry2DType getType() const {
				return type;
			}

			vec2<float32> getCentroid() const {
				return centroid;
			}

			float32 getRadius() const {
				return radius;
			}

			uint32 getVertexOffset() const {
				return vertexOffset;
			}

			view<svec2<float32>> getVertices() const {
				return vertices;
			}

			uint32 getIndexOffset() const {
				return indexOffset;
			}

			view<uint16> getTriangleListIndices() const {
				return triangleListIndices;
			}

			uint16 getTriangleListIndexCount() const {
				return triangleListIndexCount;
			}

			uint16 getLineStripIndexCount() const {
				return lineStripIndexCount;
			}

			const sequence<Edge>& getEdges() const {
				return edges;
			}

			float32 getPenetrationBias() const {
				switch (type) {
					case Geometry2DType::CIRCLE: {
						return 0.01f;
					}
					case Geometry2DType::CONVEX_HULL: {
						return 0.2f;
					}
					case Geometry2DType::POINT: {
						return 0.0f;
					}
					default: {
						DEBUG_ASSERT(false);

						return 0.0f;
					}
				}
			}

			float32 calculateMass(vec2<float32> scale, float32 density) const {
				switch (type) {
					case Geometry2DType::CIRCLE: {
						ENGINE_ASSERT(scale[0] == scale[1], "The scale must be equal in both X and Y directions.");

						return pi<float32>() * pow(radius * scale[0], 2.0f) * density;
					}
					case Geometry2DType::CONVEX_HULL: {
						// TODO(physics/inertia)
						// return scaledArea(view<vec2<float32>>(vertices), scale) * density;
						const float32 area = scale[0] * scale[0] + scale[1] * scale[1];

						return area;
					}
					case Geometry2DType::POINT: {
						return 0.0_kg;
					}
					default: {
						DEBUG_ASSERT(false);

						return 0.0_kg;
					}
				}
			}

			// Returns the farthest point on the geometry along `d`.
			vec2<float32> support(vec2<float32> d) const {
				switch (type) {
					case Geometry2DType::CIRCLE: {
						return normalized(d) * radius;
					}
					case Geometry2DType::CONVEX_HULL: {
						const svec2<float32> vertex = vertices[supportIndex(d)];

						return {vertex[0], vertex[1]};
					}
					case Geometry2DType::POINT: {
						return {0.0_m, 0.0_m};
					}
					default: {
						DEBUG_ASSERT(false);

						return {0.0_m, 0.0_m};
					}
				}
			}

			// Returns the index of the farthest point on the geometry along `d`.
			// NOTE: Some geometries like CircleGeometry2D return more precise support points. In these cases, `support` must be used.
			uint32 supportIndex(vec2<float32> d) const {
				switch (type) {
					case Geometry2DType::CIRCLE: {
						ENGINE_ASSERT(false, "Use Geometry2D::support to get a support point on a circle geometry.");

						return 0;
					}
					case Geometry2DType::CONVEX_HULL: {
						float32 maxValue = dot(vec2<float32>(vertices[0]), d);
						uint32 maxValueIndex = 0;
						uint32 vertexIndex = 1;

						while (vertexIndex < vertices.count()) {
							const float32 value = dot(vec2<float32>(vertices[vertexIndex]), d);

							if (value > maxValue) {
								maxValue = value;
								maxValueIndex = vertexIndex;
							}

							vertexIndex += 1;
						}

						return maxValueIndex;
					}
					case Geometry2DType::POINT: {
						return 0;
					}
					default: {
						DEBUG_ASSERT(false);

						return 0;
					}
				}
			}

		public:
			Geometry2DType type;

			/// Average of all vertices.
			vec2<float32> centroid;

			/// Distance between the center of mass and the farthest vertex.
			float32 radius;

			uint32 vertexOffset;
			view<svec2<float32>> vertices;

			uint32 indexOffset;
			view<uint16> triangleListIndices;
			uint16 triangleListIndexCount;
			uint16 lineStripIndexCount;

			// TODO: Store the edges of all geometries contiguously.
			sequence<Edge> edges;
	};
}