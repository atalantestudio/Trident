#include "pch.hpp"

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	Geometry2D::Geometry2D(const Geometry2DDescriptor& descriptor) :
		vertices(descriptor.vertices)
	{
		// Calculate center of mass.
		for (const svec2<float32>& vertex : vertices) {
			centerOfMass += vertex;
		}

		centerOfMass /= static_cast<float32>(vertices.size());

		// Calculate radius.
		for (const svec2<float32>& vertex : vertices) {
			radius = std::max(radius, distanceSq(vertex, centerOfMass));
		}

		radius = lyah::sqrt(radius);
	}

	const chain<svec2<float32>>& Geometry2D::getVertices() const {
		return vertices;
	}

	const svec2<float32>& Geometry2D::getCenterOfMass() const {
		return centerOfMass;
	}

	float32 Geometry2D::getRadius() const {
		return radius;
	}
}