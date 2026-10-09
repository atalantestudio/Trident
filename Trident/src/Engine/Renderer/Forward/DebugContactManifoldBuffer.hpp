// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer> ForwardRenderer::createDebugContactManifoldBuffer() const {
		return createDynamicBuffer({
			.size = DEBUG_MAX_CONTACTS * sizeof(vec3<float32>),
			.usage = BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Debug contact manifold",
			#endif
		});
	}
}