#include "pch.hpp"

#include "ProjectA/Camera/OrthographicCamera.hpp"

namespace ProjectA {
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane) :
		Camera(mat4x4<float>::orthographic(left, right, bottom, top, nearPlane, farPlane)) {}

	void OrthographicCamera::updateView(const vec3<float>& position, float rotation) {
		PROFILE_SCOPE(__FUNCSIG__);

		mat4x4<float> inverseTransform = mat4x4<float>::translate(position);
		// TODO: radians()
		inverseTransform *= mat4x4<float>::rotateZ(rotation * (float) M_PI / 180.0f);
		inverseTransform.invert();

		view = inverseTransform;
		viewProjection = projection * view;
	}

	void OrthographicCamera::updateProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
		PROFILE_SCOPE(__FUNCSIG__);

		projection = mat4x4<float>::orthographic(left, right, bottom, top, nearPlane, farPlane);
		viewProjection = projection * view;
	}
}