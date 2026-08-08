// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Image> Lightmapper::createGBufferPositionImage() const {
		return gBufferRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = imageAssetDescriptor.extent,
				.mipLevelCount = 1,
				.layerCount = 1,
				.additionalUsage = ImageUsage::STORAGE,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "G-Buffer position",
				#endif
			},
			0
		);
	}

	ref<Image> Lightmapper::createGBufferSmoothPositionImage() const {
		return gBufferRenderPass->createColorAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = imageAssetDescriptor.extent,
				.mipLevelCount = 1,
				.layerCount = 1,
				.additionalUsage = ImageUsage::STORAGE,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "G-Buffer smooth position",
				#endif
			},
			1
		);
	}

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
			2
		);
	}

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
			3
		);
	}

	ref<ImageView> Lightmapper::createGBufferImageView(ref<Image> image, view<char8> name) const {
		return image->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = name,
			#endif
		});
	}
}