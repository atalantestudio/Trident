#include "pch.hpp"

#include "Engine/Camera/OrthographicCamera3D.hpp"

namespace ProjectA {
	OrthographicCamera3D::OrthographicCamera3D(float32 left, float32 right, float32 bottom, float32 top, float32 nearPlane, float32 farPlane) :
		Camera3D(mat4x4<float32>::orthographic(left, right, bottom, top, nearPlane, farPlane))
	{}

	void OrthographicCamera3D::updateProjection(float32 left, float32 right, float32 bottom, float32 top, float32 nearPlane, float32 farPlane) {
		PROFILE_SCOPE(FUNC_SIG);

		projection = mat4x4<float32>::orthographic(left, right, bottom, top, nearPlane, farPlane);
		projectionView = projection * view;
	}

	void OrthographicCamera3D::updateView(const vec3<float32>& position, float32 rotation) {
		PROFILE_SCOPE(FUNC_SIG);

		mat4x4<float32> transform = mat4x4<float32>::rotation({0.0f, 0.0f, 1.0f}, rotation);

		// TODO: Get rid of the transpose.
		transform *= lyah::transpose(mat4x4<float32>::translation(position));

		view = lyah::inverse(transform);
		projectionView = projection * view;
	}
}