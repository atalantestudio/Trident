// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Geometry2D.hpp"

namespace Trident {
	struct ConvexHullGeometry2DDescriptor {
		sequence<vec2<float32>> vertices;
		sequence<uint32> indices;

		// Geometry edges, represented as vertex index pairs.
		sequence<Edge> edges;
	};

	class ConvexHullGeometry2D : public Geometry2D {
		public:
			// 
			//         0
			//    0 ‒‒‒‒‒‒‒ 1
			//    ¦         ¦
			// 3  ¦         ¦  1
			//    ¦         ¦
			//    3 ‒‒‒‒‒‒‒ 2
			//         2
			// 
			static ref<ConvexHullGeometry2D> unitSquare() {
				ConvexHullGeometry2DDescriptor squareGeometryDescriptor{};
				squareGeometryDescriptor.vertices = {
					{-0.5f,  0.5f}, // Top left
					{ 0.5f,  0.5f}, // Top right
					{ 0.5f, -0.5f}, // Bottom right
					{-0.5f, -0.5f}, // Bottom left
				};
				squareGeometryDescriptor.indices = {
					0, 1, 2,
					0, 2, 3,
				};
				squareGeometryDescriptor.edges = {
					{0, 1}, // Top
					{1, 2}, // Right
					{2, 3}, // Bottom
					{3, 0}, // Left
				};

				return createRef<ConvexHullGeometry2D>(squareGeometryDescriptor);
			}

		public:
			explicit ConvexHullGeometry2D(const ConvexHullGeometry2DDescriptor& descriptor) :
				edges(descriptor.edges)
			{
				indices = descriptor.indices;
				vertices = descriptor.vertices;

				type = Geometry2DType::CONVEX_HULL;
				centroid = calculateCentroid();
				radius = calculateRadius();
			}

			float32 getPenetrationBias() const final override {
				return 0.2f;
			}

			vec2<float32> calculateCentroid() const final override {
				vec2<float32> centroid;

				for (const vec2<float32>& vertex : vertices) {
					centroid += vertex;
				}

				centroid /= static_cast<float32>(getVertexCount());

				return centroid;
			}

			float32 calculateMass(vec2<float32> scale, float32 density) const final override {
				return scaledArea(view<vec2<float32>>(vertices), scale) * density;
			}

			float32 calculateRadius() const final override {
				float32 radius = 0.0f;

				for (const vec2<float32>& vertex : vertices) {
					radius = std::max(radius, distanceSquared(vertex, centroid));
				}

				radius = sqrt(radius);

				return radius;
			}

			vec2<float32> support(vec2<float32> d) const final override {
				return vertices[supportIndex(d)];
			}

			uint32 supportIndex(vec2<float32> d) const final override {
				float32 maxValue = dot(vertices[0], d);
				uint32 maxValueIndex = 0;

				for (uint32 vertexIndex = 1; vertexIndex < vertices.count(); vertexIndex += 1) {
					const float32 value = dot(vertices[vertexIndex], d);

					if (value > maxValue) {
						maxValue = value;
						maxValueIndex = vertexIndex;
					}
				}

				return maxValueIndex;
			}

		public:
			const sequence<Edge> edges;
	};
}