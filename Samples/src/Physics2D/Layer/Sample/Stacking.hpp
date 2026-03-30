// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../BaseLayer.hpp"

namespace Trident {
	class Physics2DStackingLayer : public BaseLayer {
		public:
			void build() override {
				// TODO(physics/samples): Restore sample.
				/* Scene2D& scene = pointer_cast<SampleApplication*>(getApplication())->scene;

				const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

				RigidBody2DDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0_m, -10.0_m};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = squareGeometry;
				floorDescriptor.fillColor = 0xf4fedbff;

				scene.createStaticRigidBody(floorDescriptor);

				RigidBody2DDescriptor boxDescriptor{};
				boxDescriptor.mass = 1.0_kg;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;
				boxDescriptor.fillColor = 0xe6ddffff;

				uint32 boxIndex = 0;

				while (boxIndex < 10) {
					boxDescriptor.position = {0.0_m, 0.51_m + boxIndex * 1.05f};

					scene.createDynamicRigidBody(boxDescriptor);

					boxIndex += 1;
				} */
			}
	};
}