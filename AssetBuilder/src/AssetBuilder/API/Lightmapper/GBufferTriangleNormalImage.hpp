// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Image> Lightmapper::createGBufferTriangleNormalImage() const {
		return gBufferRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = imageAssetDescriptor.extent,
				.mipLevelCount = 1,
				.layerCount = 1,
				.additionalUsage = ImageUsage::STORAGE,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "G-Buffer triangle normal",
				#endif
			},
			1
		);
	}

	ref<ImageView> Lightmapper::createGBufferTriangleNormalImageView() {
		return context.gBufferTriangleNormalImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer triangle normal",
			#endif
		});
	}
}