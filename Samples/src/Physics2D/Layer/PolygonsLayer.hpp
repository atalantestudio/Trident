#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class PolygonsLayer : public BaseLayer {
		public:
			void build() override {
				ConvexHullGeometry2DDescriptor polygonGeometryDescriptor;
				polygonGeometryDescriptor.vertices = {
					{-3.0f,   0.0f},
					{ 4.0f,   0.0f},
					{ 3.0f,  -1.0f},
					{-2.25f, -1.5f},
				};
				polygonGeometryDescriptor.indices = {
					0, 1, 2,
					0, 2, 3,
				};
				polygonGeometryDescriptor.edges = {
					{0, 1},
					{1, 2},
					{2, 3},
					{3, 0},
				};

				ConvexHullGeometry2DDescriptor triangleGeometryDescriptor;
				triangleGeometryDescriptor.vertices = {
					{0.0f, 0.0f},
					{0.0f, 1.0f},
					{6.0f, 0.0f},
				};
				triangleGeometryDescriptor.indices = {
					0, 1, 2,
				};
				triangleGeometryDescriptor.edges = {
					{0, 1},
					{1, 2},
					{2, 0},
				};

				const ref<ConvexHullGeometry2D> polygonGeometry = createRef<ConvexHullGeometry2D>(polygonGeometryDescriptor);
				const ref<ConvexHullGeometry2D> triangleGeometry = createRef<ConvexHullGeometry2D>(triangleGeometryDescriptor);

				EntityDescriptor polygonDescriptor{};
				polygonDescriptor.position = {0.45f, -0.625f};
				polygonDescriptor.friction = 0.2f;
				polygonDescriptor.geometry = polygonGeometry;
				polygonDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(polygonDescriptor);

				EntityDescriptor triangleDescriptor{};
				triangleDescriptor.position = {0.0f, 1.957f};
				triangleDescriptor.angularVelocity = radians(0.4617f);
				triangleDescriptor.friction = radians(-30.0f);
				triangleDescriptor.mass = 1.0f;
				triangleDescriptor.geometry = triangleGeometry;
				triangleDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(triangleDescriptor);

				renderer.build();
				renderer.registerScene(scene);
			}
	};
}