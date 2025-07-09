#include "pch.hpp"

#include "Engine/API/Geometry/PolytopeGeometry2D.hpp"

namespace ProjectA {
	PolytopeGeometry2D::PolytopeGeometry2D(const Geometry2DDescriptor& descriptor) :
		Geometry2D(descriptor)
	{
		centerOfMass = calculateCenterOfMass();
		radius = calculateRadius();
	}

	vec2<float32> PolytopeGeometry2D::calculateCenterOfMass() const {
		vec2<float32> centerOfMass;

		for (const vec2<float32>& vertex : vertices) {
			centerOfMass += vertex;
		}

		centerOfMass /= static_cast<float32>(getVertexCount());

		return centerOfMass;
	}

	float32 PolytopeGeometry2D::calculateMass(float32 density) const {
		return density * area(vertices);
	}

	float32 PolytopeGeometry2D::calculateRadius() const {
		float32 radius = 0.0f;

		for (const vec2<float32>& vertex : vertices) {
			radius = std::max(radius, distanceSq(vertex, centerOfMass));
		}

		radius = lyah::sqrt(radius);

		return radius;
	}

	uint32 PolytopeGeometry2D::support(const vec2<float32>& d) const {
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
}