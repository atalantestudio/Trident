// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer<uint16>> ForwardRenderer::createDebugHullWireframeBuffer() const {
		return createBuffer<uint16>({
			.size = scene->getHullLineIndexCount() * sizeof(uint16),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Debug hull wireframe",
			#endif
		});
	}
}