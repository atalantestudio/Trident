#include "pch.hpp"

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	Geometry2D::Geometry2D(const Geometry2DDescriptor& descriptor) :
		vertices(descriptor.vertices),
		indices(descriptor.indices)
	{
		// Calculate center of mass.
		for (const vec2<float32>& vertex : vertices) {
			centerOfMass += vertex;
		}

		centerOfMass /= static_cast<float32>(vertices.size());

		// Calculate radius.
		for (const vec2<float32>& vertex : vertices) {
			radius = std::max(radius, distanceSq(vertex, centerOfMass));
		}

		radius = lyah::sqrt(radius);
	}

	const uint32 Geometry2D::support(const vec2<float32>& d) const {
		float32 maxValue = dot(vertices[0], d);
		uint32 maxValueIndex = 0;

		for (uint32 vertexIndex = 1; vertexIndex < vertices.size(); vertexIndex += 1) {
			const float32 value = dot(vertices[vertexIndex], d);

			if (value > maxValue) {
				maxValue = value;
				maxValueIndex = vertexIndex;
			}
		}

		return maxValueIndex;
	}
}