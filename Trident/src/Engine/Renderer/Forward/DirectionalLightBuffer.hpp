// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer> ForwardRenderer::createDirectionalLightBuffer() const {
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		return createDynamicBuffer({
			.size = ceil(sizeof(DirectionalLight), alignment),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Directional light",
			#endif
		});
	}
}