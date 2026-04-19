// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer<svec3<float32>>> ForwardRenderer::createDebugContactManifoldBuffer() const {
		return createBuffer<svec3<float32>>({
			.size = DEBUG_MAX_CONTACTS * sizeof(svec3<float32>),
			.usage = BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Debug contact manifold",
			#endif
		});
	}
}