#pragma once

#include "Engine/API/Camera/Camera.hpp"

namespace ProjectA {
	class PerspectiveCamera : public Camera {
		private:
			static const inline vec3<float32> RIGHT = {1.0f, 0.0f, 0.0f};
			static const inline vec3<float32> UP = {0.0f, 1.0f, 0.0f};
			static const inline vec3<float32> FORWARD = {0.0f, 0.0f, 1.0f};

		public:
			const vec3<float32>& getPosition() const {
				return position;
			}

			vec3<float32> getRight() const {
				return RIGHT * conjugate(orientation);
			}

			vec3<float32> getUp() const {
				return UP * conjugate(orientation);
			}

			vec3<float32> getForward() const {
				return FORWARD * conjugate(orientation);
			}

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

			void updateView(const vec3<float32>& rotationAccumulator) {
				const quat<float32> pitch = quat<float32>::axisAngle(RIGHT, rotationAccumulator[0]);
				const quat<float32> yaw = quat<float32>::axisAngle(UP, rotationAccumulator[1]);
				const quat<float32> roll = quat<float32>::axisAngle(FORWARD, rotationAccumulator[2]);

				orientation = orientation * pitch;
				orientation = yaw * orientation;
				orientation = orientation * roll;
				orientation = normalized(orientation);

				const mat4x4<float32> viewRotationOnly = mat4x4<float32>(orientation);

				// TODO: Negate position?
				const vec3<float32> translation = -position * orientation;
				const vec4<float32> translationRow = {translation[0], translation[1], translation[2], viewRotationOnly[3][3]};

				view = {
					viewRotationOnly[0],
					viewRotationOnly[1],
					viewRotationOnly[2],
					translationRow,
				};
			}

			void updateProjection() {
				// TODO: What to do if some values haven't been set?

				projection = perspective(fieldOfView, aspectRatio, nearPlane, farPlane, 1.0f);
			}

			void updateProjectionView() {
				// TODO: Check multiplication order.
				projectionView = view * projection;
			}

		private:
			vec3<float32> position = {0.0_m, 0.0_m, -0.4_m};

			vec3<float32> forward = {0.0f, 0.0f, 0.0f};
			// TODO: Store right vector?
			vec3<float32> up = {0.0f, 0.0f, 0.0f};

			quat<float32> orientation = quat<float32>::identity();

			float32 fieldOfView = 0.0f;
			float32 aspectRatio = 0.0f;
			float32 nearPlane = 0.001f;
			float32 farPlane = 1.0f;
	};
}