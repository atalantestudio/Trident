#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class SquaresLayer : public BaseLayer {
		public:
			void build() override {
				ConvexHullGeometry2DDescriptor squareGeometryDescriptor;
				squareGeometryDescriptor.vertices = {
					{-0.5f, -0.5f},
					{-0.5f,  0.5f},
					{ 0.5f,  0.5f},
					{ 0.5f, -0.5f},
				};
				squareGeometryDescriptor.indices = {
					0, 1,
					2, 0,
					2, 3,
				};

				const ref<ConvexHullGeometry2D> squareGeometry = createRef<ConvexHullGeometry2D>(squareGeometryDescriptor);

				EntityDescriptor squareADescriptor{};
				squareADescriptor.position = {0.0f, 0.0f};
				squareADescriptor.scale = {2.0f, 2.0f};
				squareADescriptor.geometry = squareGeometry;
				squareADescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createDynamicEntity(squareADescriptor);

				EntityDescriptor squareBDescriptor{};
				// squareBDescriptor.position = {-2.0f, -2.0f};
				squareBDescriptor.position = {0.0f, 2.0f};
				squareBDescriptor.rotation = radians(30.0f);
				squareBDescriptor.scale = {2.0f, 2.0f};
				squareBDescriptor.geometry = squareGeometry;
				squareBDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(squareBDescriptor);

				renderer.build();
				renderer.registerScene(scene);
			}
	};
}