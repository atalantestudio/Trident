#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	class PointGeometry2D : public Geometry2D {
		public:
			explicit PointGeometry2D() {
				indices = {0};
				vertices = {{0.0_m, 0.0_m}};
				centroid = calculateCentroid();
				radius = calculateRadius();
			}

			float32 getPenetrationBias() const final override {
				return 0.0f;
			}

			vec2<float32> calculateCentroid() const final override {
				return {0.0_m, 0.0_m};
			}

			float32 calculateMass(float32 density, const vec2<float32>& scale) const final override {
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