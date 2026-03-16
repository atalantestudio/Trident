// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../BaseLayer.hpp"

namespace Trident {
	class Physics2DRollingBallLayer : public BaseLayer {
		public:
			void build() override {
				Scene2D& scene = pointer_cast<SampleApplication*>(getApplication())->scene;

				const ref<CircleGeometry2D> circleGeometry = CircleGeometry2D::unitCircle(32);
				const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

				Entity2DDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0_m, -10.0_m};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = squareGeometry;
				floorDescriptor.fillColor = 0xf4fedbff;

				scene.createStaticEntity(floorDescriptor);

				Entity2DDescriptor slide0Descriptor{};
				slide0Descriptor.position = {-4.0_m, 3.2_m};
				slide0Descriptor.rotation = radians(-20.0_deg);
				slide0Descriptor.scale = {20.0f, 0.5f};
				slide0Descriptor.friction = 0.2f;
				slide0Descriptor.geometry = squareGeometry;
				slide0Descriptor.fillColor = 0xf4fedbff;

				scene.createStaticEntity(slide0Descriptor);

				Entity2DDescriptor slide1Descriptor{};
				slide1Descriptor.position = {8.0_m, 0.5_m};
				slide1Descriptor.rotation = radians(20.0_deg);
				slide1Descriptor.scale = {5.0f, 0.5f};
				slide1Descriptor.friction = 0.2f;
				slide1Descriptor.geometry = squareGeometry;
				slide1Descriptor.fillColor = 0xf4fedbff;

				scene.createStaticEntity(slide1Descriptor);

				Entity2DDescriptor ballDescriptor{};
				ballDescriptor.position = {-13.0_m, 10.0_m};
				ballDescriptor.mass = 25.0_kg;
				ballDescriptor.friction = 0.2f;
				ballDescriptor.geometry = circleGeometry;
				ballDescriptor.fillColor = 0xe6ddffff;

				scene.createDynamicEntity(ballDescriptor);
			}
	};
}