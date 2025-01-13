#include "pch.hpp"

#include "ProjectA/Camera/Controller/OrthographicCameraController.hpp"
#include "ProjectA/Input.hpp"

namespace ProjectA {
	OrthographicCameraController::OrthographicCameraController(float aspectRatio) :
		aspectRatio(aspectRatio),
		zoomLevel(1),
		position(0, 0, 0),
		speed(zoomLevel),
		rotation(0),
		rotationSpeed(90),
		camera(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1, 1) {}

	void OrthographicCameraController::update(float deltaTime) {
		PROFILE_SCOPE(__FUNCSIG__);

		if (Input::isKeyPressed(KeyCode::A)) {
			position[0] -= speed * deltaTime;
		} else if (Input::isKeyPressed(KeyCode::D)) {
			position[0] += speed * deltaTime;
		}

		if (Input::isKeyPressed(KeyCode::W)) {
			position[1] += speed * deltaTime;
		} else if (Input::isKeyPressed(KeyCode::S)) {
			position[1] -= speed * deltaTime;
		}

		if (Input::isKeyPressed(KeyCode::Q)) {
			rotation += rotationSpeed * deltaTime;
		} else if (Input::isKeyPressed(KeyCode::E)) {
			rotation -= rotationSpeed * deltaTime;
		}

		camera.updateView(position, rotation);
	}

	void OrthographicCameraController::onEvent(Event& event) {
		PROFILE_SCOPE(__FUNCSIG__);

		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<MouseScrollEvent>(BIND(OrthographicCameraController::onMouseScroll));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(OrthographicCameraController::onWindowResize));
	}

	bool OrthographicCameraController::onMouseScroll(MouseScrollEvent& event) {
		PROFILE_SCOPE(__FUNCSIG__);

		zoomLevel -= event.getOffset().y / 2;
		zoomLevel = std::max(zoomLevel, 0.25f);

		speed = zoomLevel;

		camera.updateProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1, 1);

		return true;
	}

	bool OrthographicCameraController::onWindowResize(WindowResizeEvent& event) {
		PROFILE_SCOPE(__FUNCSIG__);

		aspectRatio = (float) event.getWidth() / (float) event.getHeight();

		camera.updateProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1, 1);

		return true;
	}
}