// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<VisibilityRenderer::Camera>> VisibilityRenderer::createCameraBuffer() const {
		BufferDescriptor cameraBuffer{};
		cameraBuffer.size = sizeof(Camera);
		cameraBuffer.usage = BufferUsage::UNIFORM;
		cameraBuffer.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		cameraBuffer.persistent = true;

		SET_DEBUG_NAME(cameraBuffer, "Camera");

		return createBuffer<Camera>(cameraBuffer);
	}
}