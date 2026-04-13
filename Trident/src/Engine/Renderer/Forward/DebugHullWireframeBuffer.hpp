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
			#if defined(BUILD_MODE_DEBUG)
				.name = "Debug hull wireframe",
			#endif
		});
	}
}