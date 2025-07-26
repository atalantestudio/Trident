#pragma once

#include "../BaseLayer.hpp"

namespace ProjectA {
	class SampleLayer : public BaseLayer {
		public:
			void build() override {
				const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

				EntityDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0f, -10.0f};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = squareGeometry;
				floorDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(floorDescriptor);

				EntityDescriptor swingBaseDescriptor{};
				swingBaseDescriptor.position = {0.0f, 0.5f};
				swingBaseDescriptor.scale = {0.1f, 1.0f};
				swingBaseDescriptor.friction = 0.0f;
				swingBaseDescriptor.geometry = squareGeometry;
				swingBaseDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(swingBaseDescriptor);

				EntityDescriptor swingDescriptor{};
				swingDescriptor.position = {0.0f, 1.0f};
				swingDescriptor.scale = {12.0f, 0.25f};
				swingDescriptor.mass = 100.0f;
				swingDescriptor.friction = 0.2f;
				swingDescriptor.geometry = squareGeometry;
				swingDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(swingDescriptor);

				EntityDescriptor leftWeight0Descriptor{};
				leftWeight0Descriptor.position = {-5.0f, 2.0f};
				leftWeight0Descriptor.scale = {0.5f, 0.5f};
				leftWeight0Descriptor.mass = 25.0f;
				leftWeight0Descriptor.friction = 0.2f;
				leftWeight0Descriptor.geometry = squareGeometry;
				leftWeight0Descriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(leftWeight0Descriptor);

				EntityDescriptor leftWeight1Descriptor{};
				leftWeight1Descriptor.position = {-5.5f, 2.0f};
				leftWeight1Descriptor.scale = {0.5f, 0.5f};
				leftWeight1Descriptor.mass = 25.0f;
				leftWeight1Descriptor.friction = 0.2f;
				leftWeight1Descriptor.geometry = squareGeometry;
				leftWeight1Descriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(leftWeight1Descriptor);

				EntityDescriptor rightWeightDescriptor{};
				rightWeightDescriptor.position = {5.5f, 15.0f};
				rightWeightDescriptor.mass = 100.0f;
				rightWeightDescriptor.friction = 0.2f;
				rightWeightDescriptor.geometry = squareGeometry;
				rightWeightDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(rightWeightDescriptor);
			}
	};
}