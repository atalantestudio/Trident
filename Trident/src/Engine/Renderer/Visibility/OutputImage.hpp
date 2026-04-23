// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Image> VisibilityRenderer::createOutputImage() const {
		const Extent2D extent = getSwapchain()->getExtent();

		return createImage({
			.type = ImageType::TEXTURE_2D,
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {
				.width = extent.width,
				.height = extent.height,
				.depth = 1,
			},
			.mipLevelCount = 1,
			.layerCount = 1,
			.sampleCount = 1,
			.usage = ImageUsage::STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Output",
			#endif
		});
	}

	ref<ImageView> VisibilityRenderer::createOutputImageView() {
		return outputImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Output",
			#endif
		});
	}
}