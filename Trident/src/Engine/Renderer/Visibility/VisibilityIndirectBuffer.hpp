// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer2<DrawIndirectCommand>> VisibilityRenderer::createVisibilityIndirectBuffer() const {
		BufferDescriptor visibilityIndirectBufferDescriptor{};
		visibilityIndirectBufferDescriptor.size = sizeof(DrawIndirectCommand);
		visibilityIndirectBufferDescriptor.usage = BufferUsage::INDIRECT;
		visibilityIndirectBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		visibilityIndirectBufferDescriptor.persistent = false;

		SET_DEBUG_NAME(visibilityIndirectBufferDescriptor, "Visibility indirect");

		return createBuffer2<DrawIndirectCommand>(visibilityIndirectBufferDescriptor);
	}
}