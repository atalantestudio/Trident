#include "pch.hpp"

#include "Engine/API/Geometry/PointGeometry2D.hpp"

namespace ProjectA {
	PointGeometry2D::PointGeometry2D(const Geometry2DDescriptor& descriptor) :
		Geometry2D(descriptor)
	{
		centerOfMass = computeCenterOfMass();
		radius = computeRadius();
	}

	vec2<float32> PointGeometry2D::computeCenterOfMass() const {
		return vertices[0];
	}

	float32 PointGeometry2D::computeRadius() const {
		return 0.0f;
	}

	uint32 PointGeometry2D::support(const vec2<float32>& d) const {
		return 0;
	}
}