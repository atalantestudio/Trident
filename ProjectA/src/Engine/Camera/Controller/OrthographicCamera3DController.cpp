#include "pch.hpp"

#include "Engine/Camera/Controller/OrthographicCamera3DController.hpp"

namespace ProjectA {
	OrthographicCamera3DController::OrthographicCamera3DController(float32 aspectRatio) :
		aspectRatio(aspectRatio),
		zoomLevel(1),
		position(0, 0, 0),
		speed(zoomLevel),
		rotation(0),
		rotationSpeed(1),
		camera(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1, 1),
		bounds(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel) {}

	void OrthographicCamera3DController::setZoomLevel(float32 zoomLevel) {
		this->zoomLevel = zoomLevel;

		updateProjection();
	}

	void OrthographicCamera3DController::resize(const vec2<float32>& size) {
		aspectRatio = size[0] / size[1];

		camera.updateProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1, 1);
	}

	void OrthographicCamera3DController::update(float32 deltaTime) {
		PROFILE_SCOPE(FUNC_SIG);

		float32 x = 0.0f;
		float32 y = 0.0f;

		// TODO: Implement Input.
		/*if (Input::isKeyPressed(KeyCode::A)) {
			x -= speed * deltaTime;
		}
		else if (Input::isKeyPressed(KeyCode::D)) {
			x += speed * deltaTime;
		}

		if (Input::isKeyPressed(KeyCode::W)) {
			y += speed * deltaTime;
		}
		else if (Input::isKeyPressed(KeyCode::S)) {
			y -= speed * deltaTime;
		}

		if (Input::isKeyPressed(KeyCode::Q)) {
			rotation += rotationSpeed * deltaTime;
		}
		else if (Input::isKeyPressed(KeyCode::E)) {
			rotation -= rotationSpeed * deltaTime;
		}*/

		position += {x, y, 0.0f};

		camera.updateView(position, rotation);
	}

	void OrthographicCamera3DController::onEvent(Event& event) {
		PROFILE_SCOPE(FUNC_SIG);

		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<MouseWheelEvent>(BIND(OrthographicCamera3DController::onMouseWheel));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(OrthographicCamera3DController::onWindowResize));
	}

	bool OrthographicCamera3DController::onMouseWheel(MouseWheelEvent& event) {
		PROFILE_SCOPE(FUNC_SIG);

		zoomLevel -= static_cast<float32>(event.getDelta()) * 0.5f;
		zoomLevel = std::max(zoomLevel, 0.25f);
		speed = zoomLevel;

		updateProjection();

		return true;
	}

	bool OrthographicCamera3DController::onWindowResize(WindowResizeEvent& event) {
		PROFILE_SCOPE(FUNC_SIG);

		resize({static_cast<float32>(event.getWidth()), static_cast<float32>(event.getHeight())});

		return true;
	}

	void OrthographicCamera3DController::updateProjection() {
		bounds = {-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel};

		camera.updateProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1.0f, 1.0f);
	}
}