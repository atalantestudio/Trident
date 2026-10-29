// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	ref<Image> MapRenderer::createDepthStencilImage() const {
		const Extent2D extent = getSwapchain()->getExtent();

		return renderPass->createDepthStencilAttachmentImage(
			{
				.type = ImageType::TYPE_2D,
				.extent = {extent.x, extent.y, 1},
				.mipLevelCount = 1,
				.layerCount = 1,
				.additionalUsage = {},
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Depth stencil",
				#endif
			},
			0
		);
	}

	ref<ImageView> MapRenderer::createDepthStencilImageView() const {
		return depthStencilImage->createView({
			.type = ImageViewType::TYPE_2D,
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Depth stencil view",
			#endif
		});
	}
}