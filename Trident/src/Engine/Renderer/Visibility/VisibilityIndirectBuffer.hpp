// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<DrawIndirectCommand>> VisibilityRenderer::createVisibilityIndirectBuffer() const {
		return createBuffer<DrawIndirectCommand>({
			.size = sizeof(DrawIndirectCommand),
			.usage = BufferUsage::INDIRECT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Visibility indirect",
			#endif
		});
	}
}