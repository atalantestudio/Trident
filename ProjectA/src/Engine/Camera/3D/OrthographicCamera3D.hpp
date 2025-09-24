#pragma once

#include "Engine/Camera/3D/Camera3D.hpp"

namespace ProjectA {
	class OrthographicCamera3D : public Camera3D {
		public:
			explicit OrthographicCamera3D(float32 left, float32 right, float32 bottom, float32 top, float32 nearPlane, float32 farPlane) :
				Camera3D(mat4x4<float32>::orthographic(left, right, bottom, top, nearPlane, farPlane))
			{}

			void updateProjection(float32 left, float32 right, float32 bottom, float32 top, float32 nearPlane, float32 farPlane) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				projection = mat4x4<float32>::orthographic(left, right, bottom, top, nearPlane, farPlane);
				projectionView = projection * view;
			}

			// `rotation` is in radians.
			void updateView(const vec3<float32>& position, float32 rotation) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				mat4x4<float32> transform = mat4x4<float32>::rotation({0.0f, 0.0f, 1.0f}, rotation);

				// TODO: Get rid of the transpose.
				transform *= transpose(mat4x4<float32>::translation(position));

				view = inverse(transform);
				projectionView = projection * view;
			}
	};
}