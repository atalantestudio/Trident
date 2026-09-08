// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Buffer> Lightmapper::createArgumentBuffer() const {
		return device->createDynamicBuffer({
			.size = ceil(sizeof(Arguments), limits->minUniformBufferOffsetAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Argument",
			#endif
		});
	}
}