#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class Demo7Layer : public BaseLayer {
		public:
			void build() override {
				ConvexHullGeometry2DDescriptor convexHullGeometryDescriptor;
				convexHullGeometryDescriptor.vertices = {
					{-0.5f,  0.5f},
					{ 0.5f,  0.5f},
					{ 0.5f, -0.5f},
					{-0.5f, -0.5f},
				};
				convexHullGeometryDescriptor.indices = {
					0, 1,
					2, 0,
					2, 3,
				};
				convexHullGeometryDescriptor.edges = {
					{0, 1},
					{1, 2},
					{2, 3},
					{3, 0},
				};

				const ref<ConvexHullGeometry2D> convexHullGeometry = createRef<ConvexHullGeometry2D>(convexHullGeometryDescriptor);

				EntityDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0f, -10.0f};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = convexHullGeometry;
				floorDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(floorDescriptor);

				EntityDescriptor entity0Descriptor{};
				entity0Descriptor.position = {-1.5f, 10.0f};
				entity0Descriptor.scale = {12.0f, 0.5f};
				entity0Descriptor.friction = 0.2f;
				entity0Descriptor.geometry = convexHullGeometry;
				entity0Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity0Descriptor);

				EntityDescriptor entity1Descriptor{};
				entity1Descriptor.position = {1.0f, 6.0f};
				entity1Descriptor.rotation = 0.3f;
				entity1Descriptor.scale = {14.0f, 0.5f};
				entity1Descriptor.friction = 0.2f;
				entity1Descriptor.geometry = convexHullGeometry;
				entity1Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity1Descriptor);

				EntityDescriptor entity2Descriptor{};
				entity2Descriptor.position = {-7.0f, 4.0f};
				entity2Descriptor.scale = {0.5f, 3.0f};
				entity2Descriptor.friction = 0.2f;
				entity2Descriptor.geometry = convexHullGeometry;
				entity2Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(entity2Descriptor);

				for (uint32 dominoIndex = 0; dominoIndex < 10; dominoIndex += 1) {
					EntityDescriptor dominoDescriptor{};
					dominoDescriptor.position = {-6.0f + dominoIndex * 1.0f, 11.125f};
					dominoDescriptor.scale = {0.2f, 2.0f};
					dominoDescriptor.friction = 0.1f;
					dominoDescriptor.mass = 10.0f;
					dominoDescriptor.geometry = convexHullGeometry;
					dominoDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

					scene.createDynamicEntity(dominoDescriptor);
				}

				EntityDescriptor pusherDescriptor{};
				pusherDescriptor.position = {-7.5f, 12.0f};
				pusherDescriptor.linearVelocity = {4.0f, 0.0f};
				pusherDescriptor.scale = {0.5f, 2.0f};
				pusherDescriptor.friction = 0.2f;
				pusherDescriptor.mass = 10.0f;
				pusherDescriptor.geometry = convexHullGeometry;
				pusherDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createDynamicEntity(pusherDescriptor);
			}
	};
}