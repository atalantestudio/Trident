// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Buffer<ClusteredInstance>> VisibilityRenderer::createInstanceBuffer() const {
		ENGINE_ASSERT(scene != nullptr, "No scene was set.");

		return createBuffer<ClusteredInstance>({
			.size = scene->instances.count() * sizeof(ClusteredInstance),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Instance",
			#endif
		});
	}
}