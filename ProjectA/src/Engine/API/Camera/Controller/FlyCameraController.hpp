#pragma once

#include "Engine/API/Camera/Controller/CameraController.hpp"
#include "Engine/API/Camera/PerspectiveCamera.hpp"

namespace ProjectA {
	class FlyCameraController : public CameraController<PerspectiveCamera> {
		public:
			FlyCameraController() {
				camera.setPosition({0.0_m, 0.0_m, -0.4_m});
				camera.setFieldOfView(radians(60.0_deg));
				camera.setNearPlane(0.001_m);
				camera.setFarPlane(100.0_m);
			}

			void setAspectRatio(float32 aspectRatio) {
				camera.setAspectRatio(aspectRatio);
			}

			void updateProjection() {
				camera.updateProjection();
			}

			void update(float32 deltaTime) override {
				// camera.updatePosition(linearMotionOffset * deltaTime);
				camera.updateOrientation(angularMotionOffset * deltaTime);

				camera.updateView();
				camera.updateProjectionView();

				// linearMotionOffset = {0.0_m, 0.0_m, 0.0_m};
				angularMotionOffset = {0.0_rad, 0.0_rad, 0.0_rad};
			}

			/* void addLinearMotion(const vec3<float32>& motion) {
				linearMotionOffset += motion;
			} */

			// NOTE: `motion` must be given in radians.
			void addAngularMotion(const vec3<float32>& motion) {
				angularMotionOffset += motion;
			}

		private:
			// vec3<float32> linearMotionOffset = {0.0_m, 0.0_m, 0.0_m};
			vec3<float32> angularMotionOffset = {0.0_rad, 0.0_rad, 0.0_rad};
	};
}