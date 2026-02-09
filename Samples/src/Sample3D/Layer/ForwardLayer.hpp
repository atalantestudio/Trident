// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/Controller/FlyCameraController.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/Forward/Renderer.hpp"

#include "../Scene/Lemons.hpp"

namespace Trident {
	class Sample3DLayer : public Layer {
		private:
			static constexpr float32 MOVEMENT_SPEED = 0.5f;

		public:
			void build() override {
				renderer.setLayer(this);
				renderer.build();

				scene = createScene(getApplication()->getAssetLibrary());

				renderer.setScene(&scene);

				cameraController.setAspectRatio(getApplication()->getWindow()->getAspectRatio());
				cameraController.updateProjection();

				renderer.setCameraController<PerspectiveCamera>(&cameraController);
			}

			void update(float32 deltaTime, float32 inverseDeltaTime) override {
				move(deltaTime);

				cameraController.update(deltaTime);
			}

			void preRender() override {
				renderer.updateCamera();
			}

			void render(float64 remainingFrameTime, ref<CommandEncoder> commandEncoder) const override {
				renderer.render(commandEncoder);
			}

			void onEvent(Event& event) override {
				EventDispatcher eventDispatcher(event);
				eventDispatcher.dispatch<KeyDownEvent>(BIND(onKeyDown));
				eventDispatcher.dispatch<MouseMoveEvent>(BIND(onMouseMove));
				eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));

				renderer.onEvent(event);
			}

		private:
			bool onKeyDown(KeyDownEvent& event) {
				switch (event.getKeyCode()) {
					case KeyCode::SPACE: {
						const Window* window = getApplication()->getWindow();

						if (!window->isPointerLocked()) {
							window->lockPointer();
						}
						else {
							window->unlockPointer();
						}

						break;
					}
					default:
						break;
				}

				return true;
			}

			bool onMouseMove(MouseMoveEvent& event) {
				if (!getApplication()->getWindow()->isPointerLocked()) {
					return true;
				}

				const float32 pitch = static_cast<float32>(event.getY());
				const float32 yaw = static_cast<float32>(event.getX());
				const vec3<float32> rotation = {pitch, yaw, 0.0_deg};

				cameraController.addAngularMotion(radians(rotation));

				return true;
			}

			bool onWindowResize(WindowResizeEvent& event) {
				cameraController.setAspectRatio(getApplication()->getWindow()->getAspectRatio());
				cameraController.updateProjection();

				renderer.resize(event);

				return true;
			}

			void move(float32 deltaTime) {
				const Window& window = *getApplication()->getWindow();

				if (!window.isPointerLocked()) {
					return;
				}

				PerspectiveCamera& camera = cameraController.getCamera();

				const float32 forward = static_cast<float32>(window.isKeyDown(KeyCode::Z));
				const float32 back = static_cast<float32>(window.isKeyDown(KeyCode::S));
				const float32 right = static_cast<float32>(window.isKeyDown(KeyCode::D));
				const float32 left = static_cast<float32>(window.isKeyDown(KeyCode::Q));

				const vec3<float32> wishvel = camera.getForward() * (forward - back) + camera.getRight() * (right - left);

				if (lengthSquared(wishvel) == 0.0f) {
					return;
				}

				const vec3<float32> wishdir = normalized(wishvel) * MOVEMENT_SPEED * deltaTime;

				camera.setPosition(camera.getPosition() + wishdir);
			}

		private:
			ForwardRenderer renderer;
			Scene3D scene;
			FlyCameraController cameraController;
	};
}