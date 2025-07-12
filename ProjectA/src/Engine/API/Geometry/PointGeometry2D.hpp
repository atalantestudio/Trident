#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	class PointGeometry2D : public Geometry2D {
		public:
			explicit PointGeometry2D() {
				indices = {0};
				vertices = {{0.0f, 0.0f}};
				centerOfMass = calculateCenterOfMass();
				radius = calculateRadius();
			}

			vec2<float32> calculateCenterOfMass() const final override {
				return vertices[0];
			}

			float32 calculateMass(float32 density) const final override {
				return 0_kg;
			}

			float32 calculateRadius() const final override {
				return 0.0f;
			}

			vec2<float32> support(const vec2<float32>& d) const final override {
				return vertices[supportIndex(d)];
			}

			uint32 supportIndex(const vec2<float32>& d) const final override {
				return 0;
			}
	};
}