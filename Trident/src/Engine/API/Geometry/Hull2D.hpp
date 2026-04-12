// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Hull2DEdge.hpp"
#include "Engine/API/Geometry/Hull2DType.hpp"

namespace Trident {
	class Hull2D;
	class GeometryAsset;

	template<>
	struct Descriptor<Hull2D> {
		Hull2DType type;
		svec2<float32> centroid;
		float32 radius;

		// TODO(hull/vertex-duplication)
		// Circle, sphere and capsule hulls don't need vertices and convex hulls use a subset of the geometry's vertices.
		// However polygon hulls may have vertices that don't appear on the geometry (e.g. an AABB hull wrapping a cylinder geometry such as a glass).
		view<svec2<float32>> vertices;

		range<uint32, uint32> lineIndexRange;
	};

	class Hull2D {
		public:
			/// Creates and returns a point hull.
			/// When coupled with a rigid body, point hulls can represent the position of a 2D point (e.g. the pointer's position).
			static Hull2D point();

		public:
			explicit Hull2D() = default;

			// TODO: Only used by the asset builder.
			explicit Hull2D(const Descriptor<Hull2D>& descriptor);

			explicit Hull2D(ref<GeometryAsset> geometryAsset, view<svec2<float32>> vertices, uint32 lineIndexOffset);

			inline Hull2DType getType() const {
				return type;
			}

			inline svec2<float32> getCentroid() const {
				return centroid;
			}

			inline float32 getRadius() const {
				return radius;
			}

			inline view<svec2<float32>> getVertices() const {
				return vertices;
			}

			inline range<uint32, uint32> getLineIndexRange() const {
				return lineIndexRange;
			}

			inline const sequence<Edge>& getEdges() const {
				return edges;
			}

			inline Hull2DEdge getEdge(uint32 index) const {
				Hull2DEdge edge{};
				edge.a = vertices[edges[index].indexA];
				edge.b = vertices[edges[index].indexB];
				edge.normal = edgeNormalClockwise(edge.a, edge.b);

				return edge;
			}

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
					default: {
						DEBUG_ASSERT(false);

						return 0;
					}
				}
			}

		private:
			Hull2DType type;

			/// Average of all vertices.
			svec2<float32> centroid;

			/// Distance between the center of mass and the farthest vertex.
			float32 radius;

			// Vertex positions on the parent geometry.
			view<svec2<float32>> vertices;

			// TODO: 2D and 3D hulls should use 16-bit indices.
			range<uint32, uint32> lineIndexRange;

			// TODO(physics/hull-edges): Replace by a view inside a contiguous buffer.
			sequence<Edge> edges;
	};
}