// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::createFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor;
		framebufferDescriptor.renderPass = ref_cast<RenderPass>(renderPass);
		framebufferDescriptor.extent = swapchain->getExtent();

		if (SAMPLE_COUNT != ImageSampleCount::x1) {
			framebufferDescriptor.imageViews = {layerInput->swapchainImageView, depthStencilImageView, nullptr};
		}
		else {
			framebufferDescriptor.imageViews = {nullptr, depthStencilImageView};
		}

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 framebufferIndex = 0;

		while (framebufferIndex < framebuffers.count()) {
			#if defined(DEBUG_OBJECT_NAME)
				name = Logger::format("In flight []", framebufferIndex);

				framebufferDescriptor.name = name;
			#endif

			if (SAMPLE_COUNT != ImageSampleCount::x1) {
				framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);
			}
			else {
				framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);
			}

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

			framebufferIndex += 1;
		}
	}
}