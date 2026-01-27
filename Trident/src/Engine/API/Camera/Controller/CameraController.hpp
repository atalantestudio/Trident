// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	template<typename CameraType>
	class CameraController {
		public:
			const CameraType& getCamera() const {
				return camera;
			}

			CameraType& getCamera() {
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