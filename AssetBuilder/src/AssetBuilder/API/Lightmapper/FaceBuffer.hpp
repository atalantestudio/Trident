// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Buffer> Lightmapper::createFaceBuffer(uint64 faceCount) const {
		const uint64 alignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

		return device->createDynamicBuffer({
			.size = ceil(faceCount * sizeof(LightmapFace), alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Face",
			#endif
		});
	}
}