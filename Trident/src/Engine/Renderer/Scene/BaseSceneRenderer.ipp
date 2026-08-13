// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/Renderer/Scene/CameraBuffer.hpp"

namespace Trident {
	template<typename CameraType>
	void BaseSceneRenderer<CameraType>::build() {
		cameraBuffer = createCameraBuffer();

		const BufferView<CameraType> cameraView = cameraBuffer->createView<CameraType>(0, sizeof(CameraType));

		cameraMappedRange = cameraView.map();
	}

	template<typename CameraType>
	void BaseSceneRenderer<CameraType>::postSetScene() {
		encodeSceneOneTimeCommands();
	}
}