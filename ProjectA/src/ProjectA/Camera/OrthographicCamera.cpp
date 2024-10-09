#include "pch.hpp"
#include "OrthographicCamera.hpp"
#include "math/mat/mat4x4.hpp"

namespace ProjectA {
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane) :
		Camera(mat4x4<float>::orthographic(left, right, bottom, top, nearPlane, farPlane)) {}

	void OrthographicCamera::updateView(const vec3<float>& position, float rotation) {
		mat4x4<float> inverseTransform = mat4x4<float>::translate(position);
		// TODO: rad() or radians()
		inverseTransform *= mat4x4<float>::rotateZ(rotation * (float) M_PI / 180.0f);
		inverseTransform.invert();

		view = inverseTransform;
		viewProjection = projection * view;
	}

	void OrthographicCamera::updateProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
		projection = mat4x4<float>::orthographic(left, right, bottom, top, nearPlane, farPlane);
		viewProjection = projection * view;
	}
}