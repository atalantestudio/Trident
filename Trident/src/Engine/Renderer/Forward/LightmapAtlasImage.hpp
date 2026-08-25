// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Image> ForwardRenderer::createLightmapAtlasImage(ref<ImageAsset> imageAsset) const {
		return createDynamicImage({
			.type = ImageType::TYPE_2D,
			.format = imageAsset->getFormat(),
			.extent = imageAsset->getExtent(),
			.mipLevelCount = imageAsset->getMipLevelCount(),
			.layerCount = imageAsset->getLayerCount(),
			.sampleCount = 1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas",
			#endif
		});
	}

	ref<ImageView> ForwardRenderer::createLightmapAtlasImageView() const {
		return lightmapAtlasImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = lightmapAtlasImage->getMipLevelCount(),
			.layerOffset = 0,
			.layerCount = lightmapAtlasImage->getLayerCount(),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas view",
			#endif
		});
	}
}