// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace Trident {
	struct CircleGeometry2DDescriptor {
		float32 radius;
		uint32 resolution;
	};

	// NOTE: This geometry is well-fitted for triangle fan rendering.
	class CircleGeometry2D : public Geometry2D {
		public:
			static ref<CircleGeometry2D> unitCircle(uint32 resolution) {
				CircleGeometry2DDescriptor descriptor{};
				descriptor.radius = 0.5f;
				descriptor.resolution = resolution;

				return createRef<CircleGeometry2D>(descriptor);
			}

		public:
			explicit CircleGeometry2D(const CircleGeometry2DDescriptor& descriptor) {
				ENGINE_ASSERT(descriptor.radius > 0.0f, "The radius must be positive.");
				ENGINE_ASSERT(descriptor.resolution > 2, "The resolution must be greater than 2.");

				type = Geometry2DType::CIRCLE;
				centroid = calculateCentroid();
				radius = descriptor.radius;

				generateVertices(descriptor.resolution);
			}

			float32 getPenetrationBias() const final override {
				return 0.01f;
			}

			vec2<float32> calculateCentroid() const final override {
				return {0.0f, 0.0f};
			}

			float32 calculateMass(float32 density, const vec2<float32>& scale) const final override {
				ENGINE_ASSERT(scale[0] == scale[1], "The scale must be equal in both X and Y directions.");

				return density * pi<float32>() * pow(radius * scale[0], 2.0f);
			}

			float32 calculateRadius() const final override {
				return radius;
			}

			vec2<float32> support(const vec2<float32>& d) const final override {
				return normalized(d) * radius;
			}

			uint32 supportIndex(const vec2<float32>& d) const final override {
				ENGINE_ASSERT(false, "Use CircleGeometry::support to get a support point.");

				return 0;
			}

		private:
			void generateVertices(uint32 resolution) {
				indices = sequence<uint32>(resolution * 3);
				vertices = sequence<vec2<float32>>(resolution);

				const float32 theta = pi<float32>() * 2.0f / resolution;

				float32 angle = 0.0f;

				for (uint32 triangleIndex = 0; triangleIndex < resolution; triangleIndex += 1) {
					indices[triangleIndex * 3 + 0] = 0;
					indices[triangleIndex * 3 + 1] = triangleIndex + 1;
					indices[triangleIndex * 3 + 2] = triangleIndex + 2;

					vertices[triangleIndex] = vec2<float32>(sin(angle), cos(angle)) * radius;

					angle += theta;
				}
			}
	};
}