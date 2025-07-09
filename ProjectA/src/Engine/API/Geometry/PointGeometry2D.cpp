#include "pch.hpp"

#include "Engine/API/Geometry/PointGeometry2D.hpp"

namespace ProjectA {
	PointGeometry2D::PointGeometry2D(const Geometry2DDescriptor& descriptor) :
		Geometry2D(descriptor)
	{
		centerOfMass = calculateCenterOfMass();
		radius = calculateRadius();
	}

	vec2<float32> PointGeometry2D::calculateCenterOfMass() const {
		return vertices[0];
	}

	float32 PointGeometry2D::calculateMass(float32 density) const {
		return 0_kg;
	}

	float32 PointGeometry2D::calculateRadius() const {
		return 0.0f;
	}

	uint32 PointGeometry2D::support(const vec2<float32>& d) const {
		return 0;
	}
}