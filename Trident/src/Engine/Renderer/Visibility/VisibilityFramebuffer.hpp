// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Framebuffer> VisibilityRenderer::createVisibilityFramebuffer() const {
		const ref<Swapchain> swapchain = getSwapchain();

		FramebufferDescriptor visibilityFramebufferDescriptor{};
		visibilityFramebufferDescriptor.renderPass = visibilityRenderPass;
		visibilityFramebufferDescriptor.imageViews = {visibilityImageView, depthStencilImageView};
		visibilityFramebufferDescriptor.extent = swapchain->getExtent();

		SET_DEBUG_NAME(visibilityFramebufferDescriptor, "Visibility");

		return createFramebuffer(visibilityFramebufferDescriptor);
	}
}