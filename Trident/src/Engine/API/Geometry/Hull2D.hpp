// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Geometry/Hull2DType.hpp"
#include "Engine/API/Physics/Cached/CachedGeometry2D.hpp"

namespace Trident {
	class Hull2D;

	template<>
	struct Descriptor<Hull2D> {
		Hull2DType type;
		svec2<float32> centroid;
		float32 radius;

		// TODO(hull/vertex-duplication)
		// Circle, sphere and capsule hulls don't need vertices and convex hulls use a subset of the geometry's vertices.
		// However polygon hulls may have vertices that don't appear on the geometry (e.g. an AABB hull wrapping a cylinder geometry such as a glass).
		view<svec2<float32>> vertices;

		uint32 edgeCount;
	};

	class Hull2D {
		public:
			static Hull2D point();

			template<Hull2DType HullType>
			static Hull2D fromGeometry(const Geometry2D& geometry);

		public:
			explicit Hull2D() = default;

			explicit Hull2D(const Descriptor<Hull2D>& descriptor);

			inline Hull2DType getType() const {
				return type;
			}

			inline svec2<float32> getCentroid() const {
				return centroid;
			}

			inline float32 getRadius() const {
				return radius;
			}

			inline uint32 getVertexOffset() const {
				return vertexOffset;
			}

			inline view<svec2<float32>> getVertices() const {
				return vertices;
			}

			inline uint32 getIndexOffset() const {
				return indexOffset;
			}

			inline const sequence<uint16>& getLineIndices() const {
				return lineIndices;
			}

			inline uint16 getLineIndexCount() const {
				return lineIndexCount;
			}

			inline const sequence<Edge>& getEdges() const {
				return edges;
			}

			inline float32 getPenetrationBias() const {
				static constexpr float32 PENETRATION_BIASES[static_cast<uint32>(Hull2DType::count)] = {
					0.1f,
					0.0f,
					0.2f,
				};

				DEBUG_ASSERT(type < Hull2DType::count);

				return PENETRATION_BIASES[static_cast<uint32>(type)];
			}

			inline float32 calculateMass(vec2<float32> scale, float32 density) const {
				// TODO: Replace with LUT.
				switch (type) {
					case Hull2DType::CIRCLE: {
						ENGINE_ASSERT(scale[0] == scale[1], "The scale must be equal in both X and Y directions.");

						return pi<float32>() * pow(radius * scale[0], 2.0f) * density;
					}
					case Hull2DType::POINT: {
						return 0.0_kg;
					}
					case Hull2DType::POLYGON: {
						// TODO(physics/inertia)
						// return scaledArea(view<vec2<float32>>(vertices), scale) * density;
						const float32 area = scale[0] * scale[0] + scale[1] * scale[1];

						return area;
					}
					default: {
						DEBUG_ASSERT(false);

						return 0.0_kg;
					}
				}
			}

			// Returns the farthest point on the geometry along `d`.
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

			// Returns the index of the farthest point on the geometry along `d`.
			// NOTE: Some geometries like CircleGeometry2D return more precise support points. In these cases, `support` must be used.
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

			uint32 vertexOffset;
			view<svec2<float32>> vertices;

			uint32 indexOffset;
			sequence<uint16> lineIndices;
			uint16 lineIndexCount;

			// TODO: Store the geometry edges in a shared contiguous buffer.
			sequence<Edge> edges;
	};
}