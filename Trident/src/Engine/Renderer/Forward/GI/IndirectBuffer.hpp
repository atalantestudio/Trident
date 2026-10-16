// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
		ref<Buffer> ForwardRenderer::createGIIndirectBuffer() const {
			return createDynamicBuffer({
				.size = sizeof(DrawIndirectCommand),
				.usage = BufferUsage::READ_WRITE_STORAGE | BufferUsage::INDIRECT,
				.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
				.persistent = true,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "GI indirect",
				#endif
			});
		}
	#endif
}