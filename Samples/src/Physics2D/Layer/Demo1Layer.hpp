#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class Demo1Layer : public BaseLayer {
		public:
			void build() override {
				ConvexHullGeometry2DDescriptor convexHullGeometryDescriptor;
				convexHullGeometryDescriptor.vertices = {
					{-0.5f, -0.5f},
					{-0.5f,  0.5f},
					{ 0.5f,  0.5f},
					{ 0.5f, -0.5f},
				};
				convexHullGeometryDescriptor.indices = {
					0, 1, 2,
					0, 2, 3,
				};

				const ref<ConvexHullGeometry2D> convexHullGeometry = createRef<ConvexHullGeometry2D>(convexHullGeometryDescriptor);

				EntityDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0f, -10.0f};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = convexHullGeometry;

				// TODO: Make the floor a static entity.
				scene.createDynamicEntity(floorDescriptor);

				for (uint32 boxIndex = 0; boxIndex < 10; boxIndex += 1) {
					EntityDescriptor boxDescriptor{};
					boxDescriptor.position = {random<float32>(-0.1f, 0.1f), 0.51f + 1.05f * boxIndex};
					boxDescriptor.mass = 1.0f;
					boxDescriptor.friction = 0.2f;
					boxDescriptor.geometry = convexHullGeometry;

					scene.createDynamicEntity(boxDescriptor);
				}

				renderer.build();
				renderer.registerScene(scene);
			}
	};
}