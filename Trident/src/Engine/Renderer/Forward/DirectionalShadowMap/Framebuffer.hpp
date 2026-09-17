// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::createDirectionalShadowMapFramebuffers() {
		const uint32 swapchainImageCount = getSwapchain()->getImageCount();

		directionalShadowMapFramebuffers = sequence<ref<Framebuffer>>(swapchainImageCount * DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT);

		FramebufferDescriptor framebufferDescriptor = {
			.renderPass = directionalShadowMapRenderPass,
			.imageViews = {nullptr},
			.extent = DIRECTIONAL_SHADOW_MAP_EXTENT,
		};

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 swapchainImageIndex = 0;

		while (swapchainImageIndex < swapchainImageCount) {
			uint32 cascadeIndex = 0;

			while (cascadeIndex < DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT) {
				#if defined(DEBUG_OBJECT_NAME)
					name = Logger::format("Directional shadow map [] (cascade [])", swapchainImageIndex, cascadeIndex);

					framebufferDescriptor.name = name;
				#endif

				// TODO(renderer/forward): A const variable must be used to prevent a change of owner.
				const ref<ImageView> imageView = directionalShadowMapCascadeImageViews[cascadeIndex];

				framebufferDescriptor.imageViews[0] = imageView;

				directionalShadowMapFramebuffers[swapchainImageIndex * swapchainImageCount + cascadeIndex] = createFramebuffer(framebufferDescriptor);

				cascadeIndex += 1;
			}

			swapchainImageIndex += 1;
		}
	}
}