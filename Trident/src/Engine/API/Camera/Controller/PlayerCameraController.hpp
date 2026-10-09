// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/3D/PerspectiveCamera3D.hpp"
#include "Engine/API/Camera/Controller/CameraController.hpp"

namespace Trident {
	class PlayerCameraController : public CameraController<PerspectiveCamera3D> {
		public:
			void move() {
				// const translation = -eye * conjugate(orientation);

				// camera.view.set(translation, 12);
			}

			void update(float32 deltaTime) override {
				camera.updateOrientation(angularMotionAccumulator);
				camera.updateView();
				camera.updateProjectionView();

				angularMotionAccumulator = {0.0_rad, 0.0_rad, 0.0_rad};
			}

			void accumulateAngularMotion(vec3<float32> motion) {
				angularMotionAccumulator += motion;
			}

		private:
			vec3<float32> linearMotionAccumulator = {0.0_m_s, 0.0_m_s, 0.0_m_s};
			vec3<float32> angularMotionAccumulator = {0.0_rad, 0.0_rad, 0.0_rad};
	};
}