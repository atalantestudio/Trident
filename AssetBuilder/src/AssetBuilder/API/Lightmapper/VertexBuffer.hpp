// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Buffer> Lightmapper::createVertexBuffer(uint64 vertexCount) const {
		const uint64 alignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

		return device->createDynamicBuffer({
			.size = ceil(vertexCount * sizeof(BuildVertex3D), alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::READ_ONLY_ACCELERATION_STRUCTURE_INPUT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex",
			#endif
		});
	}
}