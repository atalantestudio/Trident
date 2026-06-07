// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/StaticSceneRaytracer/StaticSceneRayTracer.hpp"

namespace Trident {
	ref<Buffer<uint8>> StaticSceneRayTracer::createAccumulateBuffer() const {
		const Extent2D extent = getSwapchain()->getExtent();

		return createBuffer<uint8>({
			.size = extent.width * extent.height * sizeof(vec4<float32>),
			.usage = BufferUsage::READ_WRITE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Accumulate",
			#endif
		});
	}
}