// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Image> VisibilityRenderer::createDepthStencilImage() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		return visibilityRenderPass->createDepthStencilAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = {
					.width = swapchainExtent.width,
					.height = swapchainExtent.height,
					.depth = 1,
				},
				.mipLevelCount = 1,
				.layerCount = 1,
				#if defined(BUILD_MODE_DEBUG)
					.name = "Depth stencil",
				#endif
			},
			0
		);
	}

	ref<ImageView> VisibilityRenderer::createDepthStencilImageView() {
		return depthStencilImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Depth stencil",
			#endif
		});
	}
}