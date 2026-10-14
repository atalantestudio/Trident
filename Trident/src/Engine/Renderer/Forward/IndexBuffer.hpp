// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer> ForwardRenderer::createIndexBuffer() const {
		return createDynamicBuffer({
			.size = scene->getAsset()->indexRange.count,
			.usage = (
				#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
					BufferUsage::READ_ONLY_STORAGE |
				#endif
				BufferUsage::TRANSFER_DST | BufferUsage::INDEX
			),
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Index",
			#endif
		});
	}
}