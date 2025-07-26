#pragma once

#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	struct ConvexHullGeometry2DDescriptor {
		sequence<vec2<float32>> vertices;
		sequence<uint32> indices;

		// Geometry edges, represented as vertex index pairs.
		sequence<Edge> edges;
	};

	class ConvexHullGeometry2D : public Geometry2D {
		public:
			static ref<ConvexHullGeometry2D> unitSquare() {
				ConvexHullGeometry2DDescriptor squareGeometryDescriptor{};
				squareGeometryDescriptor.vertices = {
					{-0.5f,  0.5f},
					{ 0.5f,  0.5f},
					{ 0.5f, -0.5f},
					{-0.5f, -0.5f},
				};
				squareGeometryDescriptor.indices = {
					0, 1,
					2, 0,
					2, 3,
				};
				squareGeometryDescriptor.edges = {
					{0, 1},
					{1, 2},
					{2, 3},
					{3, 0},
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

			vec2<float32> calculateCentroid() const final override {
				vec2<float32> centroid;

				for (const vec2<float32>& vertex : vertices) {
					centroid += vertex;
				}

				centroid /= static_cast<float32>(getVertexCount());

				return centroid;
			}

			float32 calculateMass(float32 density, const vec2<float32>& scale) const final override {
				return density * scaledArea(vertices, scale);
			}

			float32 calculateRadius() const final override {
				float32 radius = 0.0f;

				for (const vec2<float32>& vertex : vertices) {
					radius = std::max(radius, distanceSq(vertex, centroid));
				}

				radius = lyah::sqrt(radius);

				return radius;
			}

			vec2<float32> support(const vec2<float32>& d) const final override {
				return vertices[supportIndex(d)];
			}

			uint32 supportIndex(const vec2<float32>& d) const final override {
				float32 maxValue = dot(vertices[0], d);
				uint32 maxValueIndex = 0;

				for (uint32 vertexIndex = 1; vertexIndex < vertices.count; vertexIndex += 1) {
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