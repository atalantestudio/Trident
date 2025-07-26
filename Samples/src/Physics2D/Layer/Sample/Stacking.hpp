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
				boxDescriptor.mass = 1.0f;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;

				for (uint32 boxIndex = 0; boxIndex < 10; boxIndex += 1) {
					boxDescriptor.position = {random<float32>(-0.1f, 0.1f), 0.51f + 1.05f * boxIndex};

					scene.createDynamicEntity(boxDescriptor);
				}
			}
	};
}