// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Axis.hpp"
#include "Engine/API/Camera/Camera.hpp"

namespace Trident {
	class PerspectiveCamera : public Camera {
		public:
			const vec3<float32>& getPosition() const {
				return position;
			}

			// NOTE: This requires recomputing the view and projection-view matrices.
			void setPosition(const vec3<float32>& position) {
				this->position = position;
			}

			const quat<float32>& getOrientation() const {
				return orientation;
			}

			// NOTE: This requires recomputing the view and projection-view matrices.
			void setOrientation(const quat<float32>& orientation) {
				this->orientation = normalized(orientation);
			}

			vec3<float32> getRight() const {
				return RIGHT * orientation;
			}

			vec3<float32> getUp() const {
				return UP * orientation;
			}

			vec3<float32> getForward() const {
				return FORWARD * orientation;
			}

			float32 getFieldOfView() const {
				return fieldOfView;
			}

			// NOTE: `fieldOfView` must be given in radians.
			// NOTE: This requires recomputing the projection and projection-view matrices.
			void setFieldOfView(float32 fieldOfView) {
				DEBUG_ASSERT(fieldOfView > 0.0_rad);

				this->fieldOfView = fieldOfView;
			}

			float32 getAspectRatio() const {
				return aspectRatio;
			}

			// NOTE: This requires recomputing the projection and projection-view matrices.
			void setAspectRatio(float32 aspectRatio) {
				DEBUG_ASSERT(aspectRatio > 0.0f);

				this->aspectRatio = aspectRatio;
			}

			float32 getNearPlane() const {
				return nearPlane;
			}

			// NOTE: This requires recomputing the projection and projection-view matrices.
			void setNearPlane(float32 nearPlane) {
				DEBUG_ASSERT(nearPlane > 0.0f);

				this->nearPlane = nearPlane;
			}

			float32 getFarPlane() const {
				return farPlane;
			}

			// NOTE: This requires recomputing the projection and projection-view matrices.
			void setFarPlane(float32 farPlane) {
				DEBUG_ASSERT(farPlane > nearPlane);

				this->farPlane = farPlane;
			}

			/* void updatePosition(const vec3<float32>& offset) {
				const vec3<float32> right = getRight() * offset[0];
				const vec3<float32> up = getUp() * offset[1];
				const vec3<float32> forward = getForward() * offset[2];

				position += right + forward + up;
			} */

			void updateOrientation(svec3<float32> offset) {
				if (dot(getUp(), UP) < 0.0f) {
					offset[1] *= -1.0f;
				}

				const quat<float32> pitch = quat<float32>::axisAngle(RIGHT, offset[0]);
				const quat<float32> yaw = quat<float32>::axisAngle(UP, offset[1]);
				const quat<float32> roll = quat<float32>::axisAngle(FORWARD, offset[2]);

				orientation = normalized((yaw * (orientation * pitch)) * roll);
			}

			void updateProjection() {
				projection = perspective(fieldOfView, aspectRatio, nearPlane, farPlane, 1.0f);
			}

			void updateView() {
				// TODO: Use forward rotation with this formula: -position * conjugate(orientation)
				const vec3<float32> translation = -(conjugate(orientation) * quat<float32>(0.0f, position[0], position[1], position[2]) * orientation).xyz();

				view = mat4x4<float32>(orientation);
				view[3] = {translation[0], translation[1], translation[2], 1.0f};
			}

		private:
			vec3<float32> position = {0.0_m, 0.0_m, 0.0_m};
			quat<float32> orientation = quat<float32>::axisAngle(FORWARD, 0.0_rad);
			float32 fieldOfView = 0.0f;
			float32 aspectRatio = 0.0f;
			float32 nearPlane = 0.001f;
			float32 farPlane = 1.0f;
	};
}