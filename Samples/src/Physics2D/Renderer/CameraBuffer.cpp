// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::createCameraBuffer() {
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		cameraBuffer = createDynamicBuffer({
			.size = ceilPowerOf2(sizeof(Camera), alignment),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Camera",
			#endif
		});

		const BufferView<Camera> cameraView = cameraBuffer->createView<Camera>(0, cameraBuffer->getSize());

		cameraMappedRange = cameraView.map();
		cameraMappedRange->projectionAndZoomLevel.m[2].w = DEFAULT_ZOOM_LEVEL;
	}
}