#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class Demo0Layer : public BaseLayer {
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

				const ref<ConvexHullGeometry2D> squareGeometry = createRef<ConvexHullGeometry2D>(convexHullGeometryDescriptor);

				EntityDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0f, -10.0f};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = squareGeometry;
				floorDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(floorDescriptor);

				EntityDescriptor boxDescriptor{};
				boxDescriptor.position = {0.0f, 4.0f};
				boxDescriptor.mass = 200.0f;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;
				boxDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(boxDescriptor);

				renderer.build();
				renderer.registerScene(scene);
			}
	};
}