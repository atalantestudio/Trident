#pragma once

#include "ProjectA/Camera/OrthographicCamera.hpp"
#include "ProjectA/Event/Event.hpp"
#include "ProjectA/Event/MouseEvent.hpp"
#include "ProjectA/Event/WindowEvent.hpp"

namespace ProjectA {
	class OrthographicCameraController {
		private:
			float aspectRatio;
			float zoomLevel;
			vec3<float> position;
			float speed;
			float rotation;
			float rotationSpeed;
			OrthographicCamera camera;

		public:
			OrthographicCameraController(float aspectRatio);

			inline const OrthographicCamera& getCamera() const {
				return camera;
			}

			void update(float deltaTime);

			void onEvent(Event& event);

		private:
			bool onMouseScroll(MouseScrollEvent& event);
			bool onWindowResize(WindowResizeEvent& event);
	};
}