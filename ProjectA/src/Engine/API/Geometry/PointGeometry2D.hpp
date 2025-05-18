#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	class PointGeometry2D : public Geometry2D {
		public:
			explicit PointGeometry2D(const Geometry2DDescriptor& descriptor);

			vec2<float32> computeCenterOfMass() const final override;

			float32 computeRadius() const final override;

			uint32 support(const vec2<float32>& d) const final override;
	};
}