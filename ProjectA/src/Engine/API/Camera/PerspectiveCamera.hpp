#pragma once

#include "Engine/API/Camera/Camera.hpp"

namespace ProjectA {
	class PerspectiveCamera : public Camera {
		public:
			float32 getFieldOfView() const {
				return fieldOfView;
			}

			// NOTE: `fieldOfView` must be given in radians.
			void setFieldOfView(float32 fieldOfView) {
				DEBUG_ASSERT(fieldOfView > 0.0_rad);

				this->fieldOfView = fieldOfView;
			}

			float32 getAspectRatio() const {
				return aspectRatio;
			}

			void setAspectRatio(float32 aspectRatio) {
				DEBUG_ASSERT(aspectRatio > 0.0f);

				this->aspectRatio = aspectRatio;
			}

			float32 getNearPlane() const {
				return nearPlane;
			}

			void setNearPlane(float32 nearPlane) {
				DEBUG_ASSERT(nearPlane > 0.0f);

				this->nearPlane = nearPlane;
			}

			float32 getFarPlane() const {
				return farPlane;
			}

			void setFarPlane(float32 farPlane) {
				DEBUG_ASSERT(farPlane > nearPlane);

				this->farPlane = farPlane;
			}

			void updateView() {
				// TODO

				projectionView = projection * view;
			}

			void updateProjection() {
				// TODO: What to do if some values haven't been set?

				projection = perspective(fieldOfView, aspectRatio, nearPlane, farPlane, 1.0f);
				projectionView = projection * view;
			}

		private:
			vec3<float32> position = {0.0_m, 0.0_m, 0.0_m};

			vec3<float32> forward = {0.0f, 0.0f, 0.0f};
			// TODO: Store right vector?
			vec3<float32> up = {0.0f, 0.0f, 0.0f};

			// quat<float32> orientation;

			float32 fieldOfView = 0.0f;
			float32 aspectRatio = 0.0f;
			float32 nearPlane = 0.001f;
			float32 farPlane = 1.0f;
	};
}