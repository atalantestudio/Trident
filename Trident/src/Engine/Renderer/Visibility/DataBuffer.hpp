// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<uint8>> VisibilityRenderer::createDataBuffer(uint64 size) const {
		return createBuffer<uint8>({
			.size = size,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Data",
			#endif
		});
	}
}