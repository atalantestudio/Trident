// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer> VisibilityRenderer::createMaterialCoordinateBuffer() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		BufferDescriptor materialCoordinateBufferDescriptor{};
		materialCoordinateBufferDescriptor.size = swapchainExtent.width * swapchainExtent.height * sizeof(uint32);
		materialCoordinateBufferDescriptor.usage = BufferUsage::READ_WRITE_STORAGE;
		materialCoordinateBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		materialCoordinateBufferDescriptor.persistent = false;

		SET_DEBUG_NAME(materialCoordinateBufferDescriptor, "Material coordinate");

		return createBuffer(materialCoordinateBufferDescriptor);
	}
}