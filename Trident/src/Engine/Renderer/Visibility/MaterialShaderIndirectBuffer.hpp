// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<DispatchIndirectCommand>> VisibilityRenderer::createMaterialShaderIndirectBuffer() const {
		return createBuffer<DispatchIndirectCommand>({
			.size = scene->materialShaders.size() * sizeof(DispatchIndirectCommand),
			.usage = BufferUsage::READ_WRITE_STORAGE | BufferUsage::INDIRECT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Material shader indirect",
			#endif
		});
	}
}