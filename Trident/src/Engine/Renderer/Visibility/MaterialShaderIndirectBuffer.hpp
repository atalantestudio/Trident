// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<DispatchIndirectCommand>> VisibilityRenderer::createMaterialShaderIndirectBuffer() const {
		const uint64 minStorageBufferOffsetAlignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return createBuffer<DispatchIndirectCommand>({
			.size = ceil(scene->materialShaders.size() * sizeof(DispatchIndirectCommand), minStorageBufferOffsetAlignment),
			.usage = BufferUsage::READ_WRITE_STORAGE | BufferUsage::INDIRECT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Material shader indirect",
			#endif
		});
	}
}