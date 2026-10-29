// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	ref<Buffer> MapRenderer::createCameraBuffer() const {
		const uint64 alignment = getDevice()->getPhysicalDevice()->getLimits().minUniformBufferOffsetAlignment;

		return createStaticBuffer({
			.size = ceilPowerOf2(sizeof(CameraUniform), alignment),
			.usage = BufferUsage::UNIFORM,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = true,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Camera",
			#endif
		});
	}

	void MapRenderer::createCameraBuffers() {
		const uint32 swapchainImageCount = getSwapchain()->getImageCount();

		cameraBuffers = sequence<ref<Buffer>>(swapchainImageCount);
		cameraViews = sequence<BufferView<CameraUniform>>(swapchainImageCount);
		cameraMappedRanges = sequence<CameraUniform*>(swapchainImageCount);

		uint32 swapchainImageIndex = 0;

		while (swapchainImageIndex < swapchainImageCount) {
			cameraBuffers[swapchainImageIndex] = createCameraBuffer();
			cameraViews[swapchainImageIndex] = cameraBuffers[swapchainImageIndex]->createView<CameraUniform>(0, cameraBuffers[swapchainImageIndex]->getSize());

			swapchainImageIndex += 1;
		}
	}
}