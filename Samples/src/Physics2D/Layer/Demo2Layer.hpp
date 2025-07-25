#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class Demo2Layer : public BaseLayer {
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
					0, 1, 2,
					0, 2, 3,
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

				scene.createStaticEntity(floorDescriptor);

				EntityDescriptor entity0Descriptor{};
				entity0Descriptor.position = {-2.0f, 11.0f};
				entity0Descriptor.rotation = -0.25f;
				entity0Descriptor.scale = {13.0f, 0.25f};
				entity0Descriptor.friction = 0.2f;
				entity0Descriptor.geometry = convexHullGeometry;

				scene.createStaticEntity(entity0Descriptor);

				EntityDescriptor entity1Descriptor{};
				entity1Descriptor.position = {5.25f, 9.5f};
				entity1Descriptor.scale = {0.25f, 1.0f};
				entity1Descriptor.friction = 0.2f;
				entity1Descriptor.geometry = convexHullGeometry;

				scene.createStaticEntity(entity1Descriptor);

				EntityDescriptor entity2Descriptor{};
				entity2Descriptor.position = {2.0f, 7.0f};
				entity2Descriptor.rotation = 0.25f;
				entity2Descriptor.scale = {13.0f, 0.25f};
				entity2Descriptor.friction = 0.2f;
				entity2Descriptor.geometry = convexHullGeometry;

				scene.createStaticEntity(entity2Descriptor);

				EntityDescriptor entity3Descriptor{};
				entity3Descriptor.position = {-5.25f, 5.5f};
				entity3Descriptor.scale = {0.25f, 1.0f};
				entity3Descriptor.friction = 0.2f;
				entity3Descriptor.geometry = convexHullGeometry;

				scene.createStaticEntity(entity3Descriptor);

				EntityDescriptor entity4Descriptor{};
				entity4Descriptor.position = {-2.0f, 3.0f};
				entity4Descriptor.rotation = -0.25f;
				entity4Descriptor.scale = {13.0f, 0.25f};
				entity4Descriptor.friction = 0.2f;
				entity4Descriptor.geometry = convexHullGeometry;

				scene.createStaticEntity(entity4Descriptor);

				static constexpr float32 frictions[5] = {0.75f, 0.5f, 0.35f, 0.1f, 0.0f};

				for (uint32 boxIndex = 0; boxIndex < 5; boxIndex += 1) {
					EntityDescriptor boxDescriptor{};
					boxDescriptor.position = {-7.5f + 2.0f * boxIndex, 14.0f};
					boxDescriptor.scale = {0.5f, 0.5f};
					boxDescriptor.mass = 25.0f;
					boxDescriptor.friction = frictions[boxIndex];
					boxDescriptor.geometry = convexHullGeometry;

					scene.createDynamicEntity(boxDescriptor);
				}
			}
	};
}