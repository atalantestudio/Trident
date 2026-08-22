// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Buffer> Lightmapper::createEmissiveTriangleIndexBuffer(uint64 emissiveTriangleIndexCount) const {
		const uint64 alignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

		return device->createDynamicBuffer({
			.size = ceil(max(1ull, emissiveTriangleIndexCount) * sizeof(uint32), alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Emissive triangle index",
			#endif
		});
	}
}