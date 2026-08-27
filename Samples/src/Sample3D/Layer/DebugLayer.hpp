// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/Controller/NoClipCameraController.hpp"
#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/Debug/Renderer.hpp"

namespace Trident {
	class Sample3DDebugLayer : public Layer {
		private:
			static constexpr float32 MOVEMENT_SPEED = 120.0f;
			static constexpr float32 LOOK_SENSITIVITY = 0.075f;

		public:
			void initialize() override {
				Application* application = getApplication();

				const view<char8> sceneAssetName = application->getPropertyFile()->getProperty<view<char8>>("AssetName");
				const ref<Scene3DAsset> sceneAsset = application->getAssetLibrary()->getAsset<Scene3DAsset>(sceneAssetName);

				PerspectiveCamera3D& camera = cameraController.getCamera();
				camera.position = {0.0f, 64.0f, 0.0f};
				camera.fieldOfView = radians(70.0_deg);
				camera.aspectRatio = getApplication()->getWindow()->getAspectRatio();
				camera.nearPlane = 0.01f;
				camera.farPlane = 2048.0f;

				camera.updateProjectionReversedZ();
				camera.updateView();
				camera.updateProjectionView();

				renderer.setLayer(this);
				renderer.build();
				renderer.postBuild();
				renderer.setScene(sceneAsset);
				renderer.setCameraController<PerspectiveCamera3D>(&cameraController);
			}

			void encodeOneTimeCommands() override {
				renderer.encodeOneTimeCommands();
			}

			void update(float32 deltaTime, float32 inverseDeltaTime) override {
				noclipMove(deltaTime);

				cameraController.update(deltaTime);
			}

			void preRender() override {
				renderer.updateCamera();
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
				cameraController.updateProjection();

				renderer.resize(event);

				return true;
			}

			void noclipMove(float32 deltaTime) {
				const Window* window = getApplication()->getWindow();

				PerspectiveCamera3D& camera = cameraController.getCamera();

				const float32 forward = static_cast<float32>(window->isKeyDown(KeyCode::Z));
				const float32 back = static_cast<float32>(window->isKeyDown(KeyCode::S));
				const float32 right = static_cast<float32>(window->isKeyDown(KeyCode::D));
				const float32 left = static_cast<float32>(window->isKeyDown(KeyCode::Q));

				const vec3<float32> wishvel = camera.getForward() * (forward - back) + camera.getRight() * (right - left);

				if (lengthSquared(wishvel) == 0.0f) {
					return;
				}

				const vec3<float32> wishdir = normalized(wishvel) * MOVEMENT_SPEED * deltaTime;

				camera.position += wishdir;
			}

		private:
			DebugRenderer renderer;
			NoClipCameraController cameraController;
	};
}