// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../BaseLayer.hpp"

namespace Trident {
	class SampleLayer : public BaseLayer {
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
				entity0Descriptor.position = {-1.5_m, 10.0_m};
				entity0Descriptor.scale = {12.0f, 0.5f};
				entity0Descriptor.friction = 0.2f;
				entity0Descriptor.geometry = squareGeometry;
				entity0Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity0Descriptor);

				Entity2DDescriptor entity1Descriptor{};
				entity1Descriptor.position = {1.0_m, 6.0_m};
				entity1Descriptor.rotation = 0.3_rad;
				entity1Descriptor.scale = {14.0f, 0.5f};
				entity1Descriptor.friction = 0.2f;
				entity1Descriptor.geometry = squareGeometry;
				entity1Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity1Descriptor);

				Entity2DDescriptor entity2Descriptor{};
				entity2Descriptor.position = {-7.0_m, 4.0_m};
				entity2Descriptor.scale = {0.5f, 3.0f};
				entity2Descriptor.friction = 0.2f;
				entity2Descriptor.geometry = squareGeometry;
				entity2Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity2Descriptor);

				Entity2DDescriptor pusherDescriptor{};
				pusherDescriptor.position = {-7.5_m, 12.0_m};
				pusherDescriptor.linearVelocity = {4.0_m_s, 0.0_m_s};
				pusherDescriptor.scale = {0.5f, 2.0f};
				pusherDescriptor.friction = 0.2f;
				pusherDescriptor.mass = 10.0_kg;
				pusherDescriptor.geometry = squareGeometry;
				pusherDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createDynamicEntity(pusherDescriptor);

				Entity2DDescriptor dominoDescriptor{};
				dominoDescriptor.scale = {0.2_m, 2.0_m};
				dominoDescriptor.friction = 0.1f;
				dominoDescriptor.mass = 10.0_kg;
				dominoDescriptor.geometry = squareGeometry;
				dominoDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				for (uint32 dominoIndex = 0; dominoIndex < 10; dominoIndex += 1) {
					dominoDescriptor.position = {-6.0_m + dominoIndex, 11.125_m};

					scene.createDynamicEntity(dominoDescriptor);
				}
			}
	};
}