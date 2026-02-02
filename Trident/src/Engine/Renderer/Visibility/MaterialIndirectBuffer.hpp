// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer> VisibilityRenderer::createMaterialIndirectBuffer() const {
		BufferDescriptor materialIndirectBufferDescriptor{};
		materialIndirectBufferDescriptor.size = sizeof(DispatchIndirectCommand) * scene->materials.count();
		materialIndirectBufferDescriptor.usage = BufferUsage::READ_WRITE_STORAGE;
		materialIndirectBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
		materialIndirectBufferDescriptor.persistent = false;

		SET_DEBUG_NAME(materialIndirectBufferDescriptor, "Material indirect");

		return createBuffer(materialIndirectBufferDescriptor);
	}
}