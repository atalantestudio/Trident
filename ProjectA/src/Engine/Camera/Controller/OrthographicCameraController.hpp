#pragma once

#include "Engine/Camera/OrthographicCamera.hpp"
#include "Engine/Event/Event.hpp"
#include "Engine/Event/MouseEvent.hpp"
#include "Engine/Event/WindowEvent.hpp"

namespace ProjectA {
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
			OrthographicCameraController(float32 aspectRatio);

			inline const vec3<float32>& getPosition() const {
				return position;
			}

			inline const vec4<float32>& getBounds() const {
				return bounds;
			}

			inline const OrthographicCamera& getCamera() const {
				return camera;
			}

			void setZoomLevel(float32 zoomLevel);

			// TODO: Use vec2<int32>
			void resize(const vec2<float32>& size);

			void update(float32 deltaTime);

			void onEvent(Event& event);

		private:
			bool onMouseScroll(MouseScrollEvent& event);
			bool onWindowResize(WindowResizeEvent& event);

			void updateProjection();
	};
}