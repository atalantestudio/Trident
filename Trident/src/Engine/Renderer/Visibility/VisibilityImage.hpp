// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Image> VisibilityRenderer::createVisibilityImage() const {
		const Extent2D extent = getSwapchain()->getExtent();

		return visibilityRenderPass->createInputAttachmentImage(
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
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Visibility",
				#endif
			},
			0
		);
	}

	ref<ImageView> VisibilityRenderer::createVisibilityImageView() {
		return visibilityImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Visibility",
			#endif
		});
	}
}