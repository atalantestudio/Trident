#include "pch.hpp"

#include "Engine/Camera/Controller/OrthographicCameraController.hpp"
#include "Engine/Input.hpp"

namespace ProjectA {
	OrthographicCameraController::OrthographicCameraController(float32 aspectRatio) :
		aspectRatio(aspectRatio),
		zoomLevel(1),
		position(0, 0, 0),
		speed(zoomLevel),
		rotation(0),
		rotationSpeed(90),
		camera(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1, 1),
		bounds(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel) {}

	void OrthographicCameraController::setZoomLevel(float32 zoomLevel) {
		this->zoomLevel = zoomLevel;

		updateProjection();
	}

	void OrthographicCameraController::resize(const vec2<float32>& size) {
		aspectRatio = size[0] / size[1];

		camera.updateProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1, 1);
	}

	void OrthographicCameraController::update(float32 deltaTime) {
		PROFILE_SCOPE(FUNC_SIG);

		float32 _position[2] = {position[0], position[1]};

		if (Input::isKeyPressed(KeyCode::A)) {
			_position[0] -= speed * deltaTime;
		} else if (Input::isKeyPressed(KeyCode::D)) {
			_position[0] += speed * deltaTime;
		}

		if (Input::isKeyPressed(KeyCode::W)) {
			_position[1] += speed * deltaTime;
		} else if (Input::isKeyPressed(KeyCode::S)) {
			_position[1] -= speed * deltaTime;
		}

		if (Input::isKeyPressed(KeyCode::Q)) {
			rotation += rotationSpeed * deltaTime;
		} else if (Input::isKeyPressed(KeyCode::E)) {
			rotation -= rotationSpeed * deltaTime;
		}

		position = {_position[0], _position[1], position[2]};

		camera.updateView(position, rotation);
	}

	void OrthographicCameraController::onEvent(Event& event) {
		PROFILE_SCOPE(FUNC_SIG);

		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<MouseScrollEvent>(BIND(OrthographicCameraController::onMouseScroll));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(OrthographicCameraController::onWindowResize));
	}

	bool OrthographicCameraController::onMouseScroll(MouseScrollEvent& event) {
		PROFILE_SCOPE(FUNC_SIG);

		zoomLevel -= event.getOffset()[1] / 2;
		zoomLevel = std::max(zoomLevel, 0.25f);
		speed = zoomLevel;

		updateProjection();

		return true;
	}

	bool OrthographicCameraController::onWindowResize(WindowResizeEvent& event) {
		PROFILE_SCOPE(FUNC_SIG);

		resize({static_cast<float32>(event.getWidth()), static_cast<float32>(event.getHeight())});

		return true;
	}

	void OrthographicCameraController::updateProjection() {
		bounds = {-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel};

		camera.updateProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1, 1);
	}
}