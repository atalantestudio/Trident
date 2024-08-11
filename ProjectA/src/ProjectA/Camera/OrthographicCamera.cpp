#include "pch.hpp"
#include "OrthographicCamera.hpp"
#include "math/mat/mat4x4.hpp"

namespace ProjectA {
	void OrthographicCamera::computeView() {
		mat4x4<float> inverseTransform = mat4x4<float>::translate(position).invert();

		view = inverseTransform;
		viewProjection = projection * view;
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane) : Camera(mat4x4<float>::orthographic(left, right, bottom, top, nearPlane, farPlane)), rotation(0) {
		viewProjection = projection;
	}

	void OrthographicCamera::setPosition(const vec3<float>& position) {
		this->position = position;

		computeView();
	}

	float OrthographicCamera::getRotation() const {
		return rotation;
	}

	void OrthographicCamera::setRotation(float rotation) {
		this->rotation = rotation;

		computeView();
	}
}