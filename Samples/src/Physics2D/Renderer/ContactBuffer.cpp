// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	ref<Buffer> Physics2DRenderer::createContactBuffer(uint32 contactCount) const {
		return createStaticBuffer({
			.size = contactCount * sizeof(vec2<float32>),
			.usage = BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Contact",
			#endif
		});
	}
}