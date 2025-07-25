#pragma once

#include "../BaseLayer.hpp"

namespace ProjectA {
	class SampleLayer : public BaseLayer {
		public:
			void build() override {
				CircleGeometry2DDescriptor circleGeometryDescriptor{};
				circleGeometryDescriptor.radius = 0.5f;
				circleGeometryDescriptor.resolution = 64;

				const ref<CircleGeometry2D> circleGeometry = createRef<CircleGeometry2D>(circleGeometryDescriptor);

				ConvexHullGeometry2DDescriptor squareGeometryDescriptor{};
				squareGeometryDescriptor.vertices = {
					{-0.5f,  0.5f},
					{ 0.5f,  0.5f},
					{ 0.5f, -0.5f},
					{-0.5f, -0.5f},
				};
				squareGeometryDescriptor.indices = {
					0, 1,
					2, 0,
					2, 3,
				};
				squareGeometryDescriptor.edges = {
					{0, 1},
					{1, 2},
					{2, 3},
					{3, 0},
				};

				const ref<ConvexHullGeometry2D> squareGeometry = createRef<ConvexHullGeometry2D>(squareGeometryDescriptor);

				EntityDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0f, -10.0f};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.friction = 0.2f;
				floorDescriptor.geometry = squareGeometry;
				floorDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(floorDescriptor);

				EntityDescriptor slide0Descriptor{};
				slide0Descriptor.position = {-4.0f, 3.2f};
				slide0Descriptor.rotation = radians(-20.0f);
				slide0Descriptor.scale = {20.0f, 0.5f};
				slide0Descriptor.friction = 0.2f;
				slide0Descriptor.geometry = squareGeometry;
				slide0Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(slide0Descriptor);

				EntityDescriptor slide1Descriptor{};
				slide1Descriptor.position = {8.0f, 0.5f};
				slide1Descriptor.rotation = radians(20.0f);
				slide1Descriptor.scale = {5.0f, 0.5f};
				slide1Descriptor.friction = 0.2f;
				slide1Descriptor.geometry = squareGeometry;
				slide1Descriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(slide1Descriptor);

				EntityDescriptor ballDescriptor{};
				ballDescriptor.position = {-13.0f, 10.0f};
				ballDescriptor.mass = 25.0f;
				ballDescriptor.friction = 0.2f;
				ballDescriptor.geometry = circleGeometry;
				ballDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(ballDescriptor);
			}
	};
}