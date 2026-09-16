// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::createDirectionalShadowMapFramebuffers() {
		directionalShadowMapFramebuffers = sequence<ref<Framebuffer>>(getSwapchain()->getImageCount());

		FramebufferDescriptor framebufferDescriptor = {
			.renderPass = directionalShadowMapRenderPass,
			.imageViews = {directionalShadowMapImageView},
			.extent = DIRECTIONAL_SHADOW_MAP_EXTENT,
		};

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 framebufferIndex = 0;

		while (framebufferIndex < directionalShadowMapFramebuffers.count()) {
			#if defined(DEBUG_OBJECT_NAME)
				name = Logger::format("Directional shadow map []", framebufferIndex);

				framebufferDescriptor.name = name;
			#endif

			directionalShadowMapFramebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

			framebufferIndex += 1;
		}
	}
}