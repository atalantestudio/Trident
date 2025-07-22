#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class Demo0Layer : public BaseLayer {
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

				EntityDescriptor floorDescriptor{};
				floorDescriptor.position = {0.0f, -10.0f};
				floorDescriptor.scale = {100.0f, 20.0f};
				floorDescriptor.mass = 0.0f;
				floorDescriptor.geometry = squareGeometry;
				floorDescriptor.fillColor = {0.957f, 0.997f, 0.859f, 1.0f};

				scene.createStaticEntity(floorDescriptor);

				EntityDescriptor boxDescriptor{};
				boxDescriptor.position = {0.0f, 0.4f};
				boxDescriptor.mass = 200.0f;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;
				boxDescriptor.fillColor = {0.902f, 0.867f, 1.0f, 1.0f};

				scene.createDynamicEntity(boxDescriptor);

				renderer.build();
				renderer.registerScene(scene);
			}

			/* void onEvent(Event& event) override {
				EventDispatcher eventDispatcher(event);

				eventDispatcher.dispatch<KeyPressEvent>(BIND(onKeyPress));
				eventDispatcher.dispatch<KeyReleaseEvent>(BIND(onKeyRelease));
			}

			bool onKeyPress(KeyPressEvent& event) {
				// TODO
				ENGINE_DEBUG("KeyPress");

				return true;
			}

			bool onKeyRelease(KeyReleaseEvent& event) {
				// TODO
				ENGINE_DEBUG("KeyRelease");

				return true;
			} */
	};
}