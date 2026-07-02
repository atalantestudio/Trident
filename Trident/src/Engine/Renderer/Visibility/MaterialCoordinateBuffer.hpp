// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer> VisibilityRenderer::createMaterialCoordinateBuffer() const {
		const Extent2D extent = getSwapchain()->getExtent();

		return createDynamicBuffer({
			.size = extent.width * extent.height * sizeof(uint32),
			.usage = BufferUsage::READ_WRITE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Material coordinate",
			#endif
		});
	}
}