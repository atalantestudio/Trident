// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer> VisibilityRenderer::createCameraBuffer() const {
		const uint64 minUniformBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		return createDynamicBuffer({
			.size = ceil(sizeof(Camera), minUniformBufferOffsetAlignment),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Camera",
			#endif
		});
	}
}