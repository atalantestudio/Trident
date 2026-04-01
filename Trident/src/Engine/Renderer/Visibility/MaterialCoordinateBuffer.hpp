// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<uint32>> VisibilityRenderer::createMaterialCoordinateBuffer() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		return createBuffer<uint32>({
			.size = swapchainExtent.width * swapchainExtent.height * sizeof(uint32),
			.usage = BufferUsage::READ_WRITE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Material coordinate",
			#endif
		});
	}
}