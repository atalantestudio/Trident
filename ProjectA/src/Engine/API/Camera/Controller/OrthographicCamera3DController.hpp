#pragma once

#include "Engine/API/Camera/OrthographicCamera.hpp"
#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"

namespace ProjectA {
	// TODO: Extend CameraController<OrthographicCamera>.
	class OrthographicCameraController {
		private:
			float32 aspectRatio;
			float32 zoomLevel;
			vec3<float32> position;
			float32 speed;
			float32 rotation;
			float32 rotationSpeed;
			vec4<float32> bounds;
			OrthographicCamera camera;

		public:
			explicit OrthographicCameraController(float32 aspectRatio) :
				aspectRatio(aspectRatio),
				zoomLevel(1),
				position(0, 0, 0),
				speed(zoomLevel),
				rotation(0),
				rotationSpeed(1),
				camera(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1, 1),
				bounds(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel)
			{}

			inline const vec3<float32>& getPosition() const {
				return position;
			}

			inline const vec4<float32>& getBounds() const {
				return bounds;
			}

			inline const OrthographicCamera& getCamera() const {
				return camera;
			}

			void setZoomLevel(float32 zoomLevel) {
				this->zoomLevel = zoomLevel;

				updateProjection();
			}

			// TODO: Use vec2<sint32>.
			void resize(const vec2<float32>& size) {
				aspectRatio = size[0] / size[1];

				camera.updateProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1, 1);
			}

			void update(float32 deltaTime) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				float32 x = 0.0f;
				float32 y = 0.0f;

				// TODO: Re-implement Input.
				/* if (Input::isKeyDown(KeyCode::A)) {
					x -= speed * deltaTime;
				}
				else if (Input::isKeyDown(KeyCode::D)) {
					x += speed * deltaTime;
				}

				if (Input::isKeyDown(KeyCode::W)) {
					y += speed * deltaTime;
				}
				else if (Input::isKeyDown(KeyCode::S)) {
					y -= speed * deltaTime;
				}

				if (Input::isKeyDown(KeyCode::Q)) {
					rotation += rotationSpeed * deltaTime;
				}
				else if (Input::isKeyDown(KeyCode::E)) {
					rotation -= rotationSpeed * deltaTime;
				} */

				position += {x, y, 0.0f};

				camera.updateView(position, rotation);
			}

			void onEvent(Event& event) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				EventDispatcher eventDispatcher(event);
				eventDispatcher.dispatch<MouseWheelEvent>(BIND(OrthographicCameraController::onMouseWheel));
				eventDispatcher.dispatch<WindowResizeEvent>(BIND(OrthographicCameraController::onWindowResize));
			}

		private:
			bool onMouseWheel(MouseWheelEvent& event) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				zoomLevel -= static_cast<float32>(event.getDelta()) * 0.5f;
				zoomLevel = std::max(zoomLevel, 0.25f);
				speed = zoomLevel;

				updateProjection();

				return true;
			}

			bool onWindowResize(WindowResizeEvent& event) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				resize({static_cast<float32>(event.getWidth()), static_cast<float32>(event.getHeight())});

				return true;
			}

			void updateProjection() {
				bounds = {-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel};

				camera.updateProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1.0f, 1.0f);
			}
	};
}