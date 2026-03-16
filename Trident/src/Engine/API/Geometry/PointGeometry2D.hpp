// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace Trident {
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

			float32 calculateMass(vec2<float32> scale, float32 density) const final override {
				return 0_kg;
			}

			float32 calculateRadius() const final override {
				return 0.0f;
			}

			vec2<float32> support(vec2<float32> d) const final override {
				return vertices[supportIndex(d)];
			}

			uint32 supportIndex(vec2<float32> d) const final override {
				return 0;
			}
	};
}