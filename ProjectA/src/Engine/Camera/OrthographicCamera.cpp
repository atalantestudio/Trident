#include "pch.hpp"

#include "Engine/Camera/OrthographicCamera.hpp"

namespace ProjectA {
	OrthographicCamera::OrthographicCamera(float32 left, float32 right, float32 bottom, float32 top, float32 nearPlane, float32 farPlane) :
		Camera(lyah::mat4x4<float32>::orthographic(left, right, bottom, top, nearPlane, farPlane)) {}

	void OrthographicCamera::updateView(const lyah::vec3<float32>& position, float32 rotation) {
		PROFILE_SCOPE(FUNC_SIG);

		// LYAH TODO: Fix translation matrix: right-handed to left-handed
		lyah::mat4x4<float32> transform = lyah::transpose(lyah::mat4x4<float32>::translation(position));
		transform *= lyah::mat4x4<float32>::rotation({0.0f, 0.0f, 1.0f}, lyah::radians(rotation));

		view = lyah::inverse(transform);
		viewProjection = projection * view;
	}

	void OrthographicCamera::updateProjection(float32 left, float32 right, float32 bottom, float32 top, float32 nearPlane, float32 farPlane) {
		PROFILE_SCOPE(FUNC_SIG);

		projection = lyah::mat4x4<float32>::orthographic(left, right, bottom, top, nearPlane, farPlane);
		viewProjection = projection * view;
	}
}