// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../BaseLayer.hpp"

namespace Trident {
	class SampleLayer : public BaseLayer {
		public:
			void build() override {
				const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

				Entity2DDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0_m, -10.0_m};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = squareGeometry;
				floorDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(floorDescriptor);

				Entity2DDescriptor swingBaseDescriptor{};
				swingBaseDescriptor.position = {0.0_m, 0.5_m};
				swingBaseDescriptor.scale = {0.1f, 1.0f};
				swingBaseDescriptor.friction = 0.0f;
				swingBaseDescriptor.geometry = squareGeometry;
				swingBaseDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(swingBaseDescriptor);

				Entity2DDescriptor swingDescriptor{};
				swingDescriptor.position = {0.0_m, 1.0_m};
				swingDescriptor.scale = {12.0f, 0.25f};
				swingDescriptor.mass = 100.0_kg;
				swingDescriptor.friction = 0.2f;
				swingDescriptor.geometry = squareGeometry;
				swingDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(swingDescriptor);

				Entity2DDescriptor leftWeight0Descriptor{};
				leftWeight0Descriptor.position = {-5.0_m, 2.0_m};
				leftWeight0Descriptor.scale = {0.5f, 0.5f};
				leftWeight0Descriptor.mass = 25.0_kg;
				leftWeight0Descriptor.friction = 0.2f;
				leftWeight0Descriptor.geometry = squareGeometry;
				leftWeight0Descriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(leftWeight0Descriptor);

				Entity2DDescriptor leftWeight1Descriptor{};
				leftWeight1Descriptor.position = {-5.5_m, 2.0_m};
				leftWeight1Descriptor.scale = {0.5f, 0.5f};
				leftWeight1Descriptor.mass = 25.0_kg;
				leftWeight1Descriptor.friction = 0.2f;
				leftWeight1Descriptor.geometry = squareGeometry;
				leftWeight1Descriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(leftWeight1Descriptor);

				Entity2DDescriptor rightWeightDescriptor{};
				rightWeightDescriptor.position = {5.5_m, 15.0_m};
				rightWeightDescriptor.mass = 100.0_kg;
				rightWeightDescriptor.friction = 0.2f;
				rightWeightDescriptor.geometry = squareGeometry;
				rightWeightDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(rightWeightDescriptor);
			}
	};
}