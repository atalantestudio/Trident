#include "pch.hpp"

#include "Engine/Camera/OrthographicCamera.hpp"

namespace ProjectA {
	OrthographicCamera::OrthographicCamera(float32 left, float32 right, float32 bottom, float32 top, float32 nearPlane, float32 farPlane) :
		Camera(mat4x4<float32>::orthographic(left, right, bottom, top, nearPlane, farPlane))
	{}

	void OrthographicCamera::updateProjection(float32 left, float32 right, float32 bottom, float32 top, float32 nearPlane, float32 farPlane) {
		PROFILE_SCOPE(FUNC_SIG);

		projection = mat4x4<float32>::orthographic(left, right, bottom, top, nearPlane, farPlane);
		projectionView = projection * view;
	}

	void OrthographicCamera::updateView(const lyah::vec3<float32>& position, float32 rotation) {
		PROFILE_SCOPE(FUNC_SIG);

		mat4x4<float32> transform = mat4x4<float32>::rotation({0.0f, 0.0f, 1.0f}, rotation);

		// TODO: Get rid of transpose.
		transform *= lyah::transpose(mat4x4<float32>::translation(position));

		view = lyah::inverse(transform);
		projectionView = projection * view;
	}
}