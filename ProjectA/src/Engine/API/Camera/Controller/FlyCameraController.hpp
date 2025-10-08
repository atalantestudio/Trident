#pragma once

#include "Engine/API/Camera/Controller/CameraController.hpp"
#include "Engine/API/Camera/PerspectiveCamera.hpp"

namespace ProjectA {
	class FlyCameraController : public CameraController<PerspectiveCamera> {
		public:
			FlyCameraController() {
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
				camera.updateView(rotationAccumulator);
				camera.updateProjectionView();

				movementAccumulator = {0.0_m, 0.0_m, 0.0_m};
				rotationAccumulator = {0.0f, 0.0f, 0.0f};
			}

			void move(const vec3<float32>& offset) {
				movementAccumulator += offset;
			}

			// NOTE: `eulerAngles` must be given in radians.
			void rotate(const vec3<float32>& eulerAngles) {
				rotationAccumulator += eulerAngles;
			}

		private:
			vec3<float32> movementAccumulator = {0.0_m, 0.0_m, 0.0_m};
			vec3<float32> rotationAccumulator = {0.0f, 0.0f, 0.0f};
	};
}