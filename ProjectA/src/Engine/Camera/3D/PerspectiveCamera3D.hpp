#pragma once

#include "Engine/Camera/3D/Camera3D.hpp"

namespace ProjectA {
	class PerspectiveCamera3D : public Camera3D {
		public:
			explicit PerspectiveCamera3D(const mat4x4<float32>& projection, float32 nearPlane, float32 farPlane) :
				Camera3D(projection),
				nearPlane(nearPlane),
				farPlane(farPlane)
			{}

			float32 getNearPlane() const {
				return nearPlane;
			}

			float32 getFarPlane() const {
				return farPlane;
			}

			void updateView() {
				// TODO

				projectionView = projection * view;
			}

			void updateProjection(float32 fieldOfView, float32 aspectRatio, float32 nearPlane, float32 farPlane) {
				projection = perspective(fieldOfView, aspectRatio, nearPlane, farPlane, 1.0f);
				projectionView = projection * view;
			}

		private:
			float32 nearPlane;
			float32 farPlane;
	};
}