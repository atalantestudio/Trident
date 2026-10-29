// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	ref<Buffer> MapRenderer::createVertexBuffer() const {
		return createDynamicBuffer({
			.size = map.staticGeometry.vertices.count() * sizeof(Vertex3D),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Vertex",
			#endif
		});
	}
}