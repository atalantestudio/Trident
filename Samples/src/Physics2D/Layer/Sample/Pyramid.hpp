#pragma once

#include "../BaseLayer.hpp"

namespace ProjectA {
	class SampleLayer : public BaseLayer {
		public:
			void build() override {
				const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

				EntityDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0f, -10.0f};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = squareGeometry;

				scene.createStaticEntity(floorDescriptor);

				EntityDescriptor boxDescriptor{};
				boxDescriptor.mass = 10.0f;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;

				vec2<float32> x = {-6.0f, 0.75f};

				for (uint32 i = 0; i < 12; i += 1) {
					vec2<float32> y = x;

					for (uint32 j = i; j < 12; j += 1) {
						boxDescriptor.position = y;

						scene.createDynamicEntity(boxDescriptor);

						y += {1.125f, 0.0f};
					}

					x += {0.5625f, 2.0f};
				}
			}
	};
}