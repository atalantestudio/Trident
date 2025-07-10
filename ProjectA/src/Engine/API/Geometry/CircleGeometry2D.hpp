#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	struct CircleGeometry2DDescriptor {
		float32 radius;
		uint32 resolution;
	};

	// NOTE: This geometry is well-fitted for triangle fan rendering.
	class CircleGeometry2D : public Geometry2D {
		public:
			explicit CircleGeometry2D(const CircleGeometry2DDescriptor& descriptor) {
				ENGINE_ASSERT(descriptor.radius > 0.0f, "The radius must be positive.");
				ENGINE_ASSERT(descriptor.resolution > 2, "The resolution must be greater than 2.");

				centerOfMass = calculateCenterOfMass();
				radius = descriptor.radius;

				generateVertices(descriptor.resolution);
			}

			vec2<float32> calculateCenterOfMass() const final override {
				return {0.0f, 0.0f};
			}

			float32 calculateMass(float32 density) const final override {
				return density * lyah::pi<float32>() * radius * radius;
			}

			float32 calculateRadius() const final override {
				return radius;
			}

			uint32 support(const vec2<float32>& d) const final override {
				// TODO: Change the support function to return a vector instead of an index.
				return 0;
			}

		private:
			void generateVertices(uint32 resolution) {
				indices = sequence<uint32>(resolution * 3);
				vertices = sequence<vec2<float32>>(resolution);

				const float32 theta = lyah::pi<float32>() * 2.0f / resolution;

				float32 angle = 0.0f;

				for (uint32 triangleIndex = 0; triangleIndex < resolution; triangleIndex += 1) {
					indices[triangleIndex * 3 + 0] = 0;
					indices[triangleIndex * 3 + 1] = triangleIndex + 1;
					indices[triangleIndex * 3 + 2] = triangleIndex + 2;

					vertices[triangleIndex] = {lyah::sin(angle), lyah::cos(angle)};
					vertices[triangleIndex] *= radius;

					angle += theta;
				}
			}
	};
}