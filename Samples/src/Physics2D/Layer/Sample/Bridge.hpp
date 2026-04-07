// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TODO(physics/samples): Restore sample.
	/* class Physics2DBridgeLayer : public BaseLayer {
		public:
			void build() override {
				static constexpr uint32 TILE_COUNT = 15;
				static constexpr float32 TILE_MASS = 50.0_kg;
				static constexpr float32 FREQUENCY = 2.0f;
				static constexpr float32 OMEGA = 2.0f * pi<float32>() * FREQUENCY;
				static constexpr float32 DAMPING_RATIO = 0.7f;
				static constexpr float32 DAMPING_COEFFICIENT = 2.0f * TILE_MASS * DAMPING_RATIO * OMEGA;
				static constexpr float32 SPRING_STIFFNESS = TILE_MASS * OMEGA * OMEGA;

				const float32 fixedDeltaTime = static_cast<float32>(getApplication()->getFixedDeltaTime());
				const float32 divisor = DAMPING_COEFFICIENT + fixedDeltaTime * SPRING_STIFFNESS;
				const float32 biasFactor = (fixedDeltaTime * SPRING_STIFFNESS) / divisor;
				const float32 softness = 1.0f / divisor;

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
				boxDescriptor.scale = {1.0f, 0.25f};
				boxDescriptor.mass = TILE_MASS;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;
				boxDescriptor.fillColor = 0xe6ddffff;

				uint32 tileIndex = 0;

				while (tileIndex < TILE_COUNT) {
					boxDescriptor.position = {-8.5_m + tileIndex * 1.25f, 5.0_m};

					scene.createDynamicRigidBody(boxDescriptor);

					tileIndex += 1;
				}

				boxDescriptor.position = {0.0f, 1.0f};
				boxDescriptor.scale = {2.0f, 2.0f};
				boxDescriptor.mass = 200.0_kg;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;
				boxDescriptor.fillColor = 0xe6ddffff;

				scene.createDynamicRigidBody(boxDescriptor);

				Joint2DDescriptor jointDescriptor{};
				jointDescriptor.biasFactor = biasFactor;
				jointDescriptor.softness = softness;

				tileIndex = 0;

				while (tileIndex < TILE_COUNT) {
					jointDescriptor.A = tileIndex;
					jointDescriptor.B = tileIndex + 1;
					jointDescriptor.anchor = {-9.125_m + tileIndex * 1.25f, 5.0_m};

					scene.createJoint(jointDescriptor);

					tileIndex += 1;
				}

				jointDescriptor.A = TILE_COUNT;
				jointDescriptor.B = 0;
				jointDescriptor.anchor = {-9.125_m + TILE_COUNT * 1.25f, 5.0_m};

				scene.createJoint(jointDescriptor);
			}
	}; */
}