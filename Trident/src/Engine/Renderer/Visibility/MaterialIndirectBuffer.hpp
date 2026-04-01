// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<DispatchIndirectCommand>> VisibilityRenderer::createMaterialIndirectBuffer() const {
		return createBuffer<DispatchIndirectCommand>({
			.size = scene->materials.count() * sizeof(DispatchIndirectCommand),
			.usage = BufferUsage::READ_WRITE_STORAGE | BufferUsage::INDIRECT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Material indirect",
			#endif
		});
	}
}