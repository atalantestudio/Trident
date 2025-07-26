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
				floorDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(floorDescriptor);

				EntityDescriptor entity0Descriptor{};
				entity0Descriptor.position = {-1.5f, 10.0f};
				entity0Descriptor.scale = {12.0f, 0.5f};
				entity0Descriptor.friction = 0.2f;
				entity0Descriptor.geometry = squareGeometry;
				entity0Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity0Descriptor);

				EntityDescriptor entity1Descriptor{};
				entity1Descriptor.position = {1.0f, 6.0f};
				entity1Descriptor.rotation = 0.3f;
				entity1Descriptor.scale = {14.0f, 0.5f};
				entity1Descriptor.friction = 0.2f;
				entity1Descriptor.geometry = squareGeometry;
				entity1Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity1Descriptor);

				EntityDescriptor entity2Descriptor{};
				entity2Descriptor.position = {-7.0f, 4.0f};
				entity2Descriptor.scale = {0.5f, 3.0f};
				entity2Descriptor.friction = 0.2f;
				entity2Descriptor.geometry = squareGeometry;
				entity2Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity2Descriptor);

				EntityDescriptor pusherDescriptor{};
				pusherDescriptor.position = {-7.5f, 12.0f};
				pusherDescriptor.linearVelocity = {4.0f, 0.0f};
				pusherDescriptor.scale = {0.5f, 2.0f};
				pusherDescriptor.friction = 0.2f;
				pusherDescriptor.mass = 10.0f;
				pusherDescriptor.geometry = squareGeometry;
				pusherDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createDynamicEntity(pusherDescriptor);

				EntityDescriptor dominoDescriptor{};
				dominoDescriptor.scale = {0.2f, 2.0f};
				dominoDescriptor.friction = 0.1f;
				dominoDescriptor.mass = 10.0f;
				dominoDescriptor.geometry = squareGeometry;
				dominoDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				for (uint32 dominoIndex = 0; dominoIndex < 10; dominoIndex += 1) {
					dominoDescriptor.position = {-6.0f + dominoIndex * 1.0f, 11.125f};

					scene.createDynamicEntity(dominoDescriptor);
				}
			}
	};
}