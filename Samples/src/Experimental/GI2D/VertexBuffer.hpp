// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Experimental/GI2D/Renderer.hpp"

namespace Trident {
	ref<Buffer> GI2DExperimentalRenderer::createVertexBuffer() const {
		return createStaticBuffer({
			.size = 12 * sizeof(Vertex),
			.usage = BufferUsage::READ_WRITE_STORAGE | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex",
			#endif
		});
	}
}