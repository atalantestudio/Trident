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

			void updateView() {
				// TODO
			}

			void update(float32 deltaTime) override {
				// TODO
			}
	};
}