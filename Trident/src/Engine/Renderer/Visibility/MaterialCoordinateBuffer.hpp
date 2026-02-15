// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer> VisibilityRenderer::createMaterialCoordinateBuffer() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		BufferDescriptor materialCoordinateBufferDescriptor{};
		materialCoordinateBufferDescriptor.size = sizeof(uint32) * swapchainExtent.width * swapchainExtent.height;
		materialCoordinateBufferDescriptor.usage = BufferUsage::READ_WRITE_STORAGE;
		materialCoordinateBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
		materialCoordinateBufferDescriptor.persistent = false;

		SET_DEBUG_NAME(materialCoordinateBufferDescriptor, "Material coordinate");

		return createBuffer(materialCoordinateBufferDescriptor);
	}
}