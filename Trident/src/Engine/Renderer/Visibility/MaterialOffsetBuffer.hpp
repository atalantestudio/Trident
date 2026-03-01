// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer2<uint32>> VisibilityRenderer::createMaterialOffsetBuffer() const {
		BufferDescriptor materialOffsetBufferDescriptor{};
		materialOffsetBufferDescriptor.size = sizeof(uint32) * scene->materials.count();
		materialOffsetBufferDescriptor.usage = BufferUsage::READ_WRITE_STORAGE;
		materialOffsetBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
		materialOffsetBufferDescriptor.persistent = false;

		SET_DEBUG_NAME(materialOffsetBufferDescriptor, "Material offset");

		return createBuffer2<uint32>(materialOffsetBufferDescriptor);
	}
}