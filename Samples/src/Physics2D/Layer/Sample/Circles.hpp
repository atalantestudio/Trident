// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../BaseLayer.hpp"

namespace Trident {
	class Physics2DCirclesLayer : public BaseLayer {
		public:
			void build() override {
				Scene2D& scene = pointer_cast<SampleApplication*>(getApplication())->scene;

				const ref<CircleGeometry2D> circleGeometry = CircleGeometry2D::unitCircle(32);

				Entity2DDescriptor largeCircle0Descriptor{};
				largeCircle0Descriptor.position = {-10.0_m, -10.0_m};
				largeCircle0Descriptor.scale = {20.0f, 20.0f};
				largeCircle0Descriptor.friction = 0.2f;
				largeCircle0Descriptor.geometry = circleGeometry;
				largeCircle0Descriptor.fillColor = 0xf4fedbff;

				scene.createStaticEntity(largeCircle0Descriptor);

				Entity2DDescriptor largeCircle1Descriptor{};
				largeCircle1Descriptor.position = {10.0_m, -10.0_m};
				largeCircle1Descriptor.scale = {20.0f, 20.0f};
				largeCircle1Descriptor.friction = 0.2f;
				largeCircle1Descriptor.geometry = circleGeometry;
				largeCircle1Descriptor.fillColor = 0xf4fedbff;

				scene.createStaticEntity(largeCircle1Descriptor);

				Entity2DDescriptor smallCircleDescriptor{};
				smallCircleDescriptor.position = {9.5_m, 1.0_m};
				smallCircleDescriptor.mass = 200.0_kg;
				smallCircleDescriptor.friction = 0.2f;
				smallCircleDescriptor.geometry = circleGeometry;
				smallCircleDescriptor.fillColor = 0xe6ddffff;

				scene.createDynamicEntity(smallCircleDescriptor);
			}
	};
}