// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../BaseLayer.hpp"

namespace Trident {
	class Physics2DPyramidLayer : public BaseLayer {
		public:
			void build() override {
				Scene2D& scene = pointer_cast<SampleApplication*>(getApplication())->scene;

				const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

				RigidBody2DDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0_m, -10.0_m};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = squareGeometry;
				floorDescriptor.fillColor = 0xf4fedbff;

				scene.createStaticRigidBody(floorDescriptor);

				RigidBody2DDescriptor boxDescriptor{};
				boxDescriptor.mass = 10.0_kg;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;
				boxDescriptor.fillColor = 0xe6ddffff;

				vec2<float32> x = {-6.0_m, 0.75_m};
				vec2<float32> y;
				uint32 i = 0;
				uint32 j;

				while (i < 12) {
					y = x;
					j = i;

					while (j < 12) {
						boxDescriptor.position = y;

						scene.createDynamicRigidBody(boxDescriptor);

						y += {1.125_m, 0.0_m};
						j += 1;
					}

					x += {0.5625_m, 2.0_m};
					i += 1;
				}
			}
	};
}