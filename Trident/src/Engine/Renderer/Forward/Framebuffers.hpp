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
			#if FORWARD_SAMPLE_COUNT > 1
				.imageViews = {colorImageView, depthStencilImageView, nullptr},
			#else
				.imageViews = {nullptr, depthStencilImageView},
			#endif
			.extent = swapchain->getExtent(),
		};

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 framebufferIndex = 0;

		while (framebufferIndex < framebuffers.count()) {
			#if defined(DEBUG_OBJECT_NAME)
				name = Logger::format("In flight []", framebufferIndex);

				framebufferDescriptor.name = name;
			#endif

			#if FORWARD_SAMPLE_COUNT > 1
				framebufferDescriptor.imageViews[2] = swapchain->getImageView(framebufferIndex);
			#else
				framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);
			#endif

			framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

			framebufferIndex += 1;
		}
	}
}