// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Image> Lightmapper::createGBufferVertexNormalImage() const {
		return gBufferRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = imageAssetDescriptor.extent,
				.mipLevelCount = 1,
				.layerCount = 1,
				.additionalUsage = ImageUsage::STORAGE,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "G-Buffer vertex normal",
				#endif
			},
			2
		);
	}

	ref<ImageView> Lightmapper::createGBufferVertexNormalImageView() {
		return context.gBufferVertexNormalImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer vertex normal",
			#endif
		});
	}
}