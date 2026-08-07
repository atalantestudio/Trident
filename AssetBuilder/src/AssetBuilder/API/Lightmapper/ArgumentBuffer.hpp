// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Buffer> Lightmapper::createArgumentBuffer() const {
		const uint64 alignment = Renderer::getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return Renderer::createDynamicBuffer({
			.size = ceil(sizeof(Arguments), alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_WRITE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Argument",
			#endif
		});
	}
}