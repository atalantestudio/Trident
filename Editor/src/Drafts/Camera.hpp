#pragma once

namespace ProjectA {
	class GameCameraController : public CameraController<Camera3D> {
		public:
			void update() override {
				camera.update();
			}
	};
}