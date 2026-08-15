// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	void VisibilityRenderer::createPresentFramebuffers() {
		const ref<Swapchain> swapchain = getSwapchain();

		presentFramebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor presentFramebufferDescriptor = {
			.renderPass = presentRenderPass,
			.imageViews = {nullptr},
			.extent = swapchain->getExtent(),
		};

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 presentFramebufferIndex = 0;

		while (presentFramebufferIndex < presentFramebuffers.count()) {
			#if defined(DEBUG_OBJECT_NAME)
				name = Logger::format("In flight []", presentFramebufferIndex);

				presentFramebufferDescriptor.name = name;
			#endif

			presentFramebufferDescriptor.imageViews[0] = swapchain->getImageView(presentFramebufferIndex);

			presentFramebuffers[presentFramebufferIndex] = createFramebuffer(presentFramebufferDescriptor);

			presentFramebufferIndex += 1;
		}
	}
}