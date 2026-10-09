// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if FORWARD_SAMPLE_COUNT > 1
		ref<Image> ForwardRenderer::createColorImage() const {
			const Extent2D extent = getSwapchain()->getExtent();

			return renderPass->createColorAttachmentImage(
				{
					.type = ImageType::TYPE_2D,
					.extent = {extent.x, extent.y, 1},
					.mipLevelCount = 1,
					.layerCount = 1,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Color",
					#endif
				},
				0
			);
		}
	#endif

	#if FORWARD_SAMPLE_COUNT > 1
		ref<ImageView> ForwardRenderer::createColorImageView() const {
			return colorImage->createView({
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Color",
				#endif
			});
		}
	#endif
}