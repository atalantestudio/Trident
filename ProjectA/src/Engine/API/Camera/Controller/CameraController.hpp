#pragma once

namespace ProjectA {
	template<typename CameraType>
	class CameraController {
		public:
			const CameraType& getCamera() const {
				return camera;
			}

			virtual void update(float32 deltaTime) = 0;

		protected:
			CameraType camera;
	};

	// template class CameraController<Camera2D>;
	// template class CameraController<OrthographicCamera>;
	// template class CameraController<PerspectiveCamera>;
}