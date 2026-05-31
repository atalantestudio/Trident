// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Axis.hpp"
#include "Engine/API/Camera/Camera.hpp"

namespace Trident {
	class PerspectiveCamera3D : public Camera {
		public:
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

			void updateProjectionReversedZ() {
				projection = reversedZ(fieldOfView, aspectRatio, nearPlane, 1.0f);
			}

			void updateView() {
				const vec3<float32> eye = position - getForward() * distance;

				// TODO: Use forward rotation with this formula: -position * conjugate(orientation)
				const vec3<float32> translation = -(conjugate(orientation) * quat<float32>(0.0f, eye[0], eye[1], eye[2]) * orientation).xyz();

				view = mat4x4<float32>(orientation);
				view[3] = {translation[0], translation[1], translation[2], 1.0f};
			}

		public:
			/// Updating this member requires recomputing the view and projection-view matrices.
			vec3<float32> position = {0.0_m, 0.0_m, 0.0_m};

			/// Distance to the point that's being looked at.
			/// Non-zero values will have the effect of a third-person camera.
			/// Updating this member requires recomputing the view and projection-view matrices.
			float32 distance = 0.0_m;

			/// Updating this member requires recomputing the projection and projection-view matrices.
			float32 fieldOfView = 0.0_rad;

			/// Updating this member requires recomputing the projection and projection-view matrices.
			float32 aspectRatio = 1.0f / 1.0f;

			/// Updating this member requires recomputing the projection and projection-view matrices.
			float32 nearPlane = 0.001f;

			/// Updating this member requires recomputing the projection and projection-view matrices.
			float32 farPlane = 1.0f;

		private:
			quat<float32> orientation = quat<float32>::axisAngle(FORWARD, 0.0_rad);
	};
}