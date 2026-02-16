// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../BaseLayer.hpp"

namespace Trident {
	class SamplePhysics2DLayer : public BaseLayer {
		public:
			void build() override {
				const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

				Entity2DDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0_m, -10.0_m};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = squareGeometry;
				floorDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(floorDescriptor);

				Entity2DDescriptor entity0Descriptor{};
				entity0Descriptor.position = {-2.0_m, 11.0_m};
				entity0Descriptor.rotation = -0.25_rad;
				entity0Descriptor.scale = {13.0f, 0.25f};
				entity0Descriptor.friction = 0.2f;
				entity0Descriptor.geometry = squareGeometry;
				entity0Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity0Descriptor);

				Entity2DDescriptor entity1Descriptor{};
				entity1Descriptor.position = {5.25_m, 9.5_m};
				entity1Descriptor.scale = {0.25f, 1.0f};
				entity1Descriptor.friction = 0.2f;
				entity1Descriptor.geometry = squareGeometry;
				entity1Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity1Descriptor);

				Entity2DDescriptor entity2Descriptor{};
				entity2Descriptor.position = {2.0_m, 7.0_m};
				entity2Descriptor.rotation = 0.25_rad;
				entity2Descriptor.scale = {13.0f, 0.25f};
				entity2Descriptor.friction = 0.2f;
				entity2Descriptor.geometry = squareGeometry;
				entity2Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity2Descriptor);

				Entity2DDescriptor entity3Descriptor{};
				entity3Descriptor.position = {-5.25_m, 5.5_m};
				entity3Descriptor.scale = {0.25f, 1.0f};
				entity3Descriptor.friction = 0.2f;
				entity3Descriptor.geometry = squareGeometry;
				entity3Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity3Descriptor);

				Entity2DDescriptor entity4Descriptor{};
				entity4Descriptor.position = {-2.0_m, 3.0_m};
				entity4Descriptor.rotation = -0.25_rad;
				entity4Descriptor.scale = {13.0f, 0.25f};
				entity4Descriptor.friction = 0.2f;
				entity4Descriptor.geometry = squareGeometry;
				entity4Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity4Descriptor);

				static constexpr float32 frictions[5] = {0.75f, 0.5f, 0.35f, 0.1f, 0.0f};

				Entity2DDescriptor boxDescriptor{};
				boxDescriptor.scale = {0.5f, 0.5f};
				boxDescriptor.mass = 25.0_kg;
				boxDescriptor.geometry = squareGeometry;
				boxDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				for (uint32 boxIndex = 0; boxIndex < 5; boxIndex += 1) {
					boxDescriptor.position = {-7.5_m + boxIndex * 2.0f, 14.0_m};
					boxDescriptor.friction = frictions[boxIndex];

					scene.createDynamicEntity(boxDescriptor);
				}
			}
	};
}