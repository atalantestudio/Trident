// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../BaseLayer.hpp"

namespace Trident {
	class Physics2DPolygonsLayer : public BaseLayer {
		public:
			void build() override {
				ConvexHullGeometry2DDescriptor polygonGeometryDescriptor{};
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

				ConvexHullGeometry2DDescriptor triangleGeometryDescriptor{};
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

				Entity2DDescriptor polygonDescriptor{};
				polygonDescriptor.position = {0.45_m, -0.625_m};
				polygonDescriptor.friction = 0.2f;
				polygonDescriptor.geometry = polygonGeometry;
				polygonDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(polygonDescriptor);

				Entity2DDescriptor triangleDescriptor{};
				triangleDescriptor.position = {0.0_m, 1.957_m};
				triangleDescriptor.angularVelocity = radians(0.4617_w);
				triangleDescriptor.friction = radians(-30.0_deg);
				triangleDescriptor.mass = 1.0_kg;
				triangleDescriptor.geometry = triangleGeometry;
				triangleDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(triangleDescriptor);
			}
	};
}