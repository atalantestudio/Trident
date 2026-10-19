// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::createFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor = {
			.renderPass = renderPass,
			.imageViews = {},
			.extent = swapchain->getExtent(),
		};

		if (SAMPLE_COUNT != ImageSampleCount::x1) {
			framebufferDescriptor.imageViews = {colorImageView, depthStencilImageView, nullptr};
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