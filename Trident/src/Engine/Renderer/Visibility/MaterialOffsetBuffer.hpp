// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<uint32>> VisibilityRenderer::createMaterialOffsetBuffer() const {
		return createBuffer<uint32>({
			.size = scene->materials.count() * sizeof(uint32),
			.usage = BufferUsage::READ_WRITE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Material offset",
			#endif
		});
	}
}