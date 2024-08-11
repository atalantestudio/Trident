#include "pch.hpp"
#include "Camera.hpp"

namespace ProjectA {
	Camera::Camera(const mat4x4<float>& projection) :
		view(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1),
		projection(projection),
		viewProjection(projection),
		position(0, 0, 0),
		fieldOfView(0),
		aspectRatio(0) {}

	const mat4x4<float>& Camera::getView() const {
		return view;
	}

	const mat4x4<float>& Camera::getProjection() const {
		return projection;
	}

	const mat4x4<float>& Camera::getViewProjection() const {
		return viewProjection;
	}

	const vec3<float>& Camera::getPosition() const {
		return position;
	}

	void Camera::setPosition(const vec3<float>& position) {
		this->position = position;
	}

	float Camera::getFieldOfView() const {
		return fieldOfView;
	}

	float Camera::getAspectRatio() const {
		return aspectRatio;
	}
}