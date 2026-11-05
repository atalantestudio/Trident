// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	ref<Buffer> Physics2DRenderer::createTransformBuffer(uint32 transformCount) const {
		return createStaticBuffer({
			.size = transformCount * sizeof(Transform2D),
			.usage = BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Transform",
			#endif
		});
	}
}