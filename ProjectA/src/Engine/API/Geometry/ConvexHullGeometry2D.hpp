#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	struct ConvexHullGeometry2DDescriptor {
		sequence<uint32> indices;
		sequence<vec2<float32>> vertices;
	};

	class ConvexHullGeometry2D : public Geometry2D {
		public:
			explicit ConvexHullGeometry2D(const ConvexHullGeometry2DDescriptor& descriptor) {
				indices = descriptor.indices;
				vertices = descriptor.vertices;

				type = Geometry2DType::CONVEX_HULL;
				centerOfMass = calculateCenterOfMass();
				radius = calculateRadius();
			}

			vec2<float32> calculateCenterOfMass() const final override {
				vec2<float32> centerOfMass;

				for (const vec2<float32>& vertex : vertices) {
					centerOfMass += vertex;
				}

				centerOfMass /= static_cast<float32>(getVertexCount());

				return centerOfMass;
			}

			float32 calculateMass(float32 density) const final override {
				return density * area(vertices);
			}

			float32 calculateRadius() const final override {
				float32 radius = 0.0f;

				for (const vec2<float32>& vertex : vertices) {
					radius = std::max(radius, distanceSq(vertex, centerOfMass));
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
	};
}