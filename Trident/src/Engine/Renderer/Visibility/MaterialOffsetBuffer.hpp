// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer> VisibilityRenderer::createMaterialOffsetBuffer() const {
		const uint64 minStorageBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return createDynamicBuffer({
			// Ensure the buffer starts at 0 (a prefix sum will be applied on the offsets).
			.size = ceil((scene->materialShaders.count() + 1) * sizeof(uint32), minStorageBufferOffsetAlignment),

			.usage = BufferUsage::READ_WRITE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Material offset",
			#endif
		});
	}
}