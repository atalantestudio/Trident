// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../BaseLayer.hpp"

namespace Trident {
	class Physics2DContactTestLayer : public BaseLayer {
		public:
			void build() override {
				Scene2D& scene = pointer_cast<SampleApplication*>(getApplication())->scene;

				ConvexHullGeometry2DDescriptor triangleGeometryDescriptor{};
				triangleGeometryDescriptor.vertices = {
					{0.0f, 0.0f},
					{0.0f, 1.0f},
					{3.0f, 0.0f},
				};
				triangleGeometryDescriptor.indices = {
					0, 1, 2,
				};
				triangleGeometryDescriptor.edges = {
					{0, 1},
					{1, 2},
					{2, 0},
				};

				const ref<ConvexHullGeometry2D> triangleGeometry = createRef<ConvexHullGeometry2D>(triangleGeometryDescriptor);

				Entity2DDescriptor triangleDescriptor{};
				triangleDescriptor.position = {-0.0_m, -0.666_m};
				triangleDescriptor.rotation = -0.3f;
				triangleDescriptor.geometry = triangleGeometry;
				triangleDescriptor.fillColor = 0xe6ddffff;

				scene.createDynamicEntity(triangleDescriptor);
			}
	};
}