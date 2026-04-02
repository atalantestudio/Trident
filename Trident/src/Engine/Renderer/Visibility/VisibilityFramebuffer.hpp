// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Framebuffer> VisibilityRenderer::createVisibilityFramebuffer() const {
		return createFramebuffer({
			.renderPass = visibilityRenderPass,
			.imageViews = {visibilityImageView, depthStencilImageView},
			.extent = getSwapchain()->getExtent(),
			#if defined(BUILD_MODE_DEBUG)
				.name = "Visibility",
			#endif
		});
	}
}