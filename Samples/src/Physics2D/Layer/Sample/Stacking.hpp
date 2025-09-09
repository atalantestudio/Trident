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
				boxDescriptor.mass = 1.0_kg;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;

				for (uint32 boxIndex = 0; boxIndex < 10; boxIndex += 1) {
					boxDescriptor.position = {random<float32>(-0.1_m, 0.1_m), 0.51_m + boxIndex * 1.05f};

					scene.createDynamicEntity(boxDescriptor);
				}
			}
	};
}