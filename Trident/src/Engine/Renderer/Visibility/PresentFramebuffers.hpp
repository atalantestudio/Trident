// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	void VisibilityRenderer::createPresentFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		presentFramebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor presentFramebufferDescriptor{};
		presentFramebufferDescriptor.renderPass = presentRenderPass;
		presentFramebufferDescriptor.imageViews = {nullptr/*, outputImageView */};
		presentFramebufferDescriptor.extent = swapchain->getExtent();

		#if defined(BUILD_MODE_DEBUG)
			sequence<char8> name;
		#endif

		uint32 presentFramebufferIndex = 0;

		while (presentFramebufferIndex < presentFramebuffers.count()) {
			#if defined(BUILD_MODE_DEBUG)
				name = Logger::format("In flight framebuffer []", presentFramebufferIndex);

				presentFramebufferDescriptor.name = name;
			#endif

			presentFramebufferDescriptor.imageViews[0] = swapchain->getImageView(presentFramebufferIndex);

			presentFramebuffers[presentFramebufferIndex] = createFramebuffer(presentFramebufferDescriptor);

			presentFramebufferIndex += 1;
		}
	}
}