// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../BaseLayer.hpp"

namespace Trident {
	class SampleLayer : public BaseLayer {
		public:
			void build() override {
				const ref<CircleGeometry2D> circleGeometry = CircleGeometry2D::unitCircle(32);

				Entity2DDescriptor largeCircle0Descriptor{};
				largeCircle0Descriptor.position = {-10.0_m, -10.0_m};
				largeCircle0Descriptor.scale = {20.0f, 20.0f};
				largeCircle0Descriptor.friction = 0.2f;
				largeCircle0Descriptor.geometry = circleGeometry;
				largeCircle0Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(largeCircle0Descriptor);

				Entity2DDescriptor largeCircle1Descriptor{};
				largeCircle1Descriptor.position = {10.0_m, -10.0_m};
				largeCircle1Descriptor.scale = {20.0f, 20.0f};
				largeCircle1Descriptor.friction = 0.2f;
				largeCircle1Descriptor.geometry = circleGeometry;
				largeCircle1Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(largeCircle1Descriptor);

				Entity2DDescriptor smallCircleDescriptor{};
				smallCircleDescriptor.position = {9.5_m, 1.0_m};
				smallCircleDescriptor.mass = 200.0_kg;
				smallCircleDescriptor.friction = 0.2f;
				smallCircleDescriptor.geometry = circleGeometry;
				smallCircleDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(smallCircleDescriptor);
			}
	};
}