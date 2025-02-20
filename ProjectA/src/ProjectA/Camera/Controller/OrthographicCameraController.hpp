#pragma once

#include "ProjectA/Camera/OrthographicCamera.hpp"
#include "ProjectA/Event/Event.hpp"
#include "ProjectA/Event/MouseEvent.hpp"
#include "ProjectA/Event/WindowEvent.hpp"

namespace ProjectA {
	class OrthographicCameraController {
		private:
			float32 aspectRatio;
			float32 zoomLevel;
			lyah::vec3<float32> position;
			float32 speed;
			float32 rotation;
			float32 rotationSpeed;
			lyah::vec4<float32> bounds;
			OrthographicCamera camera;

		public:
			OrthographicCameraController(float32 aspectRatio);

			inline const lyah::vec3<float32>& getPosition() const {
				return position;
			}

			inline const lyah::vec4<float32>& getBounds() const {
				return bounds;
			}

			inline const OrthographicCamera& getCamera() const {
				return camera;
			}

			void update(float32 deltaTime);

			void onEvent(Event& event);

		private:
			bool onMouseScroll(MouseScrollEvent& event);
			bool onWindowResize(WindowResizeEvent& event);
	};
}