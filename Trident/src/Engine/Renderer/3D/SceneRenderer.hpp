// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/Controller/CameraController.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	template<typename SceneType>
	class SceneRenderer : public Renderer {
		public:
			void setScene(const SceneType* scene) {
				this->scene = scene;

				postSetScene();
			}

			virtual void postSetScene() = 0;

			template<typename CameraType>
			const CameraController<CameraType>& getCameraController() const {
				return *pointer_cast<const CameraController<CameraType>*>(cameraController);
			}

			template<typename CameraType>
			void setCameraController(const CameraController<CameraType>* cameraController) {
				this->cameraController = cameraController;

				postSetCameraController();
			}

			virtual void postSetCameraController() = 0;

		protected:
			const SceneType* scene;

		private:
			const void* cameraController;
	};
}