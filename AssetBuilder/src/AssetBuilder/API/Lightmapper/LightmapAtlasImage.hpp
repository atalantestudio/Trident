// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Image> Lightmapper::createLightmapAtlasImage() const {
		return dilationRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TYPE_2D,
				.extent = imageAssetDescriptor.extent,
				.mipLevelCount = imageAssetDescriptor.mipLevelCount,
				.layerCount = imageAssetDescriptor.layerCount,
				.additionalUsage = ImageUsage::TRANSFER_SRC,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Lightmap atlas",
				#endif
			},
			0
		);
	}

	ref<ImageView> Lightmapper::createLightmapAtlasImageView() {
		return context.lightmapAtlasImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = imageAssetDescriptor.mipLevelCount,
			.layerOffset = 0,
			.layerCount = imageAssetDescriptor.layerCount,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Lightmap atlas view",
			#endif
		});
	}
}