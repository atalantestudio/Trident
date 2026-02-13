// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer> VisibilityRenderer::createMaterialCountBuffer() const {
		BufferDescriptor materialCountBufferDescriptor{};
		materialCountBufferDescriptor.size = sizeof(uint32) * scene->materials.count();
		materialCountBufferDescriptor.usage = BufferUsage::READ_WRITE_STORAGE;
		materialCountBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
		materialCountBufferDescriptor.persistent = false;

		SET_DEBUG_NAME(materialCountBufferDescriptor, "Material count");

		return createBuffer(materialCountBufferDescriptor);
	}
}