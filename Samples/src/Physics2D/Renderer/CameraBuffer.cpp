// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	ref<Buffer> Physics2DRenderer::createCameraBuffer() const {
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		return createStaticBuffer({
			.size = ceilPowerOf2(sizeof(Camera), alignment),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Camera",
			#endif
		});
	}
}