#pragma once

#include "../BaseLayer.hpp"

namespace ProjectA {
	class SampleLayer : public BaseLayer {
		public:
			void build() override {
				CircleGeometry2DDescriptor largeCircleGeometryDescriptor{};
				largeCircleGeometryDescriptor.radius = 10.0f;
				largeCircleGeometryDescriptor.resolution = 64;

				const ref<CircleGeometry2D> largeCircleGeometry = createRef<CircleGeometry2D>(largeCircleGeometryDescriptor);

				CircleGeometry2DDescriptor smallCircleGeometryDescriptor{};
				smallCircleGeometryDescriptor.radius = 0.5f;
				smallCircleGeometryDescriptor.resolution = 16;

				const ref<CircleGeometry2D> smallCircleGeometry = createRef<CircleGeometry2D>(smallCircleGeometryDescriptor);

				EntityDescriptor largeCircle0Descriptor{};
				largeCircle0Descriptor.position = {-10.0f, -10.0f};
				largeCircle0Descriptor.friction = 0.2f;
				largeCircle0Descriptor.geometry = largeCircleGeometry;
				largeCircle0Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(largeCircle0Descriptor);

				EntityDescriptor largeCircle1Descriptor{};
				largeCircle1Descriptor.position = {10.0f, -10.0f};
				largeCircle1Descriptor.friction = 0.2f;
				largeCircle1Descriptor.geometry = largeCircleGeometry;
				largeCircle1Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(largeCircle1Descriptor);

				EntityDescriptor smallCircleDescriptor{};
				smallCircleDescriptor.position = {9.5f, 1.0f};
				smallCircleDescriptor.mass = 200.0f;
				smallCircleDescriptor.friction = 0.2f;
				smallCircleDescriptor.geometry = smallCircleGeometry;
				smallCircleDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(smallCircleDescriptor);
			}
	};
}