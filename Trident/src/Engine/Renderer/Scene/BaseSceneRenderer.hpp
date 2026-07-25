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

			virtual void initialize() = 0;

			virtual void updateCamera() = 0;

			void resize(WindowResizeEvent& event) final override {
				initialize();
				updateCamera();
			}

		protected:
			virtual void encodeSceneOneTimeCommands() = 0;

			VertexInputBinding getStaticGeometryVertexInputBinding() const;

		private:
			ref<Buffer> createCameraBuffer() const;
			ref<Buffer> createStaticIndexBuffer() const;
			ref<Buffer> createStaticGeometryBuffer() const;
			ref<Buffer> createStaticGeometryStagingBuffer() const;

			void copyStaticGeometry(ref<CommandEncoder> commandEncoder, ref<Buffer> staticGeometryStagingBuffer) const;

		protected:
			ref<Buffer> cameraBuffer;
			ref<Buffer> staticIndexBuffer;
			ref<Buffer> staticGeometryBuffer;

			CameraType* cameraMappedRange = nullptr;
	};
}

#include "Engine/Renderer/Scene/BaseSceneRenderer.ipp"