// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
		ref<Buffer> ForwardRenderer::createGIVertexBuffer() const {
			return createDynamicBuffer({
				.size = MAX_GI_VERTEX_COUNT * sizeof(GIVertex),
				.usage = BufferUsage::READ_WRITE_STORAGE | BufferUsage::VERTEX,
				.memoryProperties = MemoryProperty::DEVICE_LOCAL,
				.persistent = false,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "GI vertex",
				#endif
			});
		}
	#endif
}