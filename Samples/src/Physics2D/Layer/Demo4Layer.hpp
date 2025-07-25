#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class Demo4Layer : public BaseLayer {
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

				vec2<float32> x = {-6.0f, 0.75f};

				for (uint32 i = 0; i < 12; i += 1) {
					vec2<float32> y = x;

					for (uint32 j = i; j < 12; j += 1) {
						EntityDescriptor boxDescriptor{};
						boxDescriptor.position = y;
						boxDescriptor.mass = 10.0f;
						boxDescriptor.friction = 0.2f;
						boxDescriptor.geometry = convexHullGeometry;

						scene.createDynamicEntity(boxDescriptor);

						y += {1.125f, 0.0f};
					}

					x += {0.5625f, 2.0f};
				}
			}
	};
}