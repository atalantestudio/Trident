// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/3D/PerspectiveCamera3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace Trident {
	template<typename CameraType>
	class BaseSceneRenderer : public SceneRenderer<Scene3D> {
		public:
			void build() final override;

			void postSetScene() final override;

			void postSetCameraController() final override {}

			virtual void createFrameObjects() = 0;

			virtual void updateCamera() = 0;

			void resize(WindowResizeEvent& event) final override {
				createFrameObjects();
				updateCamera();
			}

		protected:
			virtual void encodeSceneOneTimeCommands() = 0;

		private:
			ref<Buffer> createCameraBuffer() const;

		protected:
			ref<Buffer> cameraBuffer;

			CameraType* cameraMappedRange = nullptr;
	};
}

#include "Engine/Renderer/Scene/BaseSceneRenderer.ipp"