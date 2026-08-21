// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/3D/PerspectiveCamera3D.hpp"
#include "Engine/API/Camera/Controller/CameraController.hpp"

namespace Trident {
	class NoClipCameraController : public CameraController<PerspectiveCamera3D> {
		private:
			static constexpr float32 ROTATION_SPEED = 5.0f;

		public:
			explicit NoClipCameraController() {
				camera.position = {0.0_m, 0.0_m, -0.4_m};
				camera.fieldOfView = radians(75.0_deg);
				camera.nearPlane = 0.01_m;
			}

			void updateProjection() {
				camera.updateProjectionReversedZ();
			}

			void update(float32 deltaTime) override {
				// camera.updatePosition(linearMotionOffset * deltaTime);
				camera.updateOrientation(svec3<float32>(angularMotionOffset /* * deltaTime */));
				camera.updateView();
				camera.updateProjectionView();

				// linearMotionOffset = {0.0_m, 0.0_m, 0.0_m};
				angularMotionOffset = {0.0_rad, 0.0_rad, 0.0_rad};
			}

			/* void addLinearMotion(vec3<float32> motion) {
				linearMotionOffset += motion;
			} */

			// NOTE: `motion` must be given in radians.
			void accumulateAngularMotion(vec3<float32> motion) {
				angularMotionOffset += motion; // * ROTATION_SPEED;
			}

		private:
			// vec3<float32> linearMotionOffset = {0.0_m, 0.0_m, 0.0_m};
			vec3<float32> angularMotionOffset = {0.0_rad, 0.0_rad, 0.0_rad};
	};
}