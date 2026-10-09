// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Image> ForwardRenderer::createDepthStencilImage() const {
		const Extent2D extent = getSwapchain()->getExtent();

		return renderPass->createDepthStencilAttachmentImage(
			{
				.type = ImageType::TYPE_2D,
				.extent = {extent.x, extent.y, 1},
				.mipLevelCount = 1,
				.layerCount = 1,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Depth stencil",
				#endif
			},
			0
		);
	}

	ref<ImageView> ForwardRenderer::createDepthStencilImageView() const {
		return depthStencilImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Depth stencil",
			#endif
		});
	}
}