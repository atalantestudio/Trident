// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	ref<Buffer> MapRenderer::createIndexBuffer() const {
		return createDynamicBuffer({
			.size = map.staticGeometry.indices.count() * sizeof(uint32),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Index",
			#endif
		});
	}
}