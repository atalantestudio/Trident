#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	class PointGeometry2D : public Geometry2D {
		public:
			explicit PointGeometry2D(const Geometry2DDescriptor& descriptor);

			vec2<float32> calculateCenterOfMass() const final override;

			float32 calculateMass(float32 density) const final override;

			float32 calculateRadius() const final override;

			uint32 support(const vec2<float32>& d) const final override;
	};
}