// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<VisibilityRenderer::Camera>> VisibilityRenderer::createCameraBuffer() const {
		return createBuffer<Camera>({
			.size = sizeof(Camera),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Camera",
			#endif
		});
	}
}