#pragma once

#include "../BaseLayer.hpp"

namespace ProjectA {
	class SampleLayer : public BaseLayer {
		public:
			void build() override {
				const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

				EntityDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0_m, -10.0_m};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = squareGeometry;

				scene.createStaticEntity(floorDescriptor);

				EntityDescriptor boxDescriptor{};
				boxDescriptor.mass = 10.0_kg;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;

				vec2<float32> x = {-6.0_m, 0.75_m};

				for (uint32 i = 0; i < 12; i += 1) {
					vec2<float32> y = x;

					for (uint32 j = i; j < 12; j += 1) {
						boxDescriptor.position = y;

						scene.createDynamicEntity(boxDescriptor);

						y += {1.125_m, 0.0_m};
					}

					x += {0.5625_m, 2.0_m};
				}
			}
	};
}