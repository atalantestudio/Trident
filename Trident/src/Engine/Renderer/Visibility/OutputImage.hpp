// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Image> VisibilityRenderer::createOutputImage() const {
		const Extent2D extent = getSwapchain()->getExtent();

		return visibilityRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = {
					.width = extent.width,
					.height = extent.height,
					.depth = 1,
				},
				.mipLevelCount = 1,
				.layerCount = 1,
				.additionalUsage = ImageUsage::STORAGE,
				#if defined(BUILD_MODE_DEBUG)
					.name = "Output",
				#endif
			},
			1
		);
	}

	ref<ImageView> VisibilityRenderer::createOutputImageView() {
		return outputImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Output",
			#endif
		});
	}
}