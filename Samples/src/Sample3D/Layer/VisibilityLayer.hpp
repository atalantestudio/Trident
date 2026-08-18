// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/Controller/FlyCameraController.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/Visibility/Renderer.hpp"

#include "../Scene/LemonAndStool.hpp"

namespace Trident {
	class Sample3DVisibilityLayer : public Layer {
		private:
			static constexpr float32 MOVEMENT_SPEED = 1.5f;
			static constexpr float32 LOOK_SENSITIVITY = 0.075f;

		public:
			~Sample3DVisibilityLayer() {
				delete scene;
			}

			void initialize() override {
				renderer.setLayer(this);
				renderer.build();

				scene = new Scene3D(LemonAndStool::initializeScene(getApplication(), renderer.getGraphicsApi(), renderer.getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment));
				clusteredScene = clusterizeScene(scene);

				renderer.setScene(&clusteredScene);

				PerspectiveCamera3D& camera = cameraController.getCamera();
				camera.aspectRatio = getApplication()->getWindow()->getAspectRatio();
				camera.farPlane = 4096.0_m;
				camera.updateProjection();
				camera.updateView();
				camera.updateProjectionView();

				renderer.setCameraController<PerspectiveCamera3D>(&cameraController);
			}

			void update(float32 deltaTime, float32 inverseDeltaTime) override {
				move(deltaTime);

				cameraController.update(deltaTime);
			}

			void preRender() override {
				renderer.updateCameraPosition();
				renderer.updateCameraProjectionView();
			}

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const override {
				renderer.render(commandEncoder);
			}

			void onEvent(Event& event) override {
				EventDispatcher eventDispatcher(event);
				eventDispatcher.dispatch<KeyDownEvent>(BIND(onKeyDown));
				eventDispatcher.dispatch<MouseDownEvent>(BIND(onMouseDown));
				eventDispatcher.dispatch<MouseMoveEvent>(BIND(onMouseMove));
				eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));

				renderer.onEvent(event);
			}

		private:
			bool onKeyDown(KeyDownEvent& event) {
				const Window* window = getApplication()->getWindow();

				if (event.getKeyCode() == KeyCode::ESCAPE && window->isPointerLocked()) {
					window->unlockPointer();
				}

				return true;
			}

			bool onMouseDown(MouseDownEvent& event) {
				const Window* window = getApplication()->getWindow();

				if (!window->isPointerLocked()) {
					window->lockPointer();
				}

				return true;
			}

			bool onMouseMove(MouseMoveEvent& event) {
				if (!getApplication()->getWindow()->isPointerLocked()) {
					return true;
				}

				const float32 pitch = event.getY() * LOOK_SENSITIVITY;
				const float32 yaw = event.getX() * LOOK_SENSITIVITY;

				cameraController.accumulateAngularMotion(radians(vec3<float32>{pitch, yaw, 0.0_deg}));

				return true;
			}

			bool onWindowResize(WindowResizeEvent& event) {
				cameraController.getCamera().aspectRatio = getApplication()->getWindow()->getAspectRatio();
				cameraController.getCamera().updateProjection();

				renderer.updateViewport();

				return true;
			}

			void move(float32 deltaTime) {
				const Window& window = *getApplication()->getWindow();

				if (!window.isPointerLocked()) {
					return;
				}

				PerspectiveCamera3D& camera = cameraController.getCamera();

				const float32 forward = static_cast<float32>(window.isKeyDown(KeyCode::Z));
				const float32 back = static_cast<float32>(window.isKeyDown(KeyCode::S));
				const float32 right = static_cast<float32>(window.isKeyDown(KeyCode::D));
				const float32 left = static_cast<float32>(window.isKeyDown(KeyCode::Q));

				const vec3<float32> wishvel = camera.getForward() * (forward - back) + camera.getRight() * (right - left);

				if (lengthSquared(wishvel) == 0.0f) {
					return;
				}

				const vec3<float32> wishdir = normalized(wishvel) * MOVEMENT_SPEED * deltaTime;

				camera.position += wishdir;
			}

		private:
			VisibilityRenderer renderer;
			Scene3D* scene;
			ClusteredScene clusteredScene;
			FlyCameraController cameraController;
	};
}