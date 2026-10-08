// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer> ForwardRenderer::createPointLightBuffer() const {
		const uint64 size = max(1ull, scene->getAsset()->pointLights.count()) * sizeof(PointLight);
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

		return createDynamicBuffer({
			.size = ceilPowerOf2(size, alignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Instance",
			#endif
		});
	}
}