// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Image> Lightmapper::createIntermediateLightmapAtlasImage() const {
		return device->createDynamicImage({
			.type = ImageType::TYPE_2D,
			.format = imageAssetDescriptor->format,
			.extent = imageAssetDescriptor->extent,
			.mipLevelCount = imageAssetDescriptor->mipLevelCount,
			.layerCount = imageAssetDescriptor->layerCount,
			.sampleCount = ImageSampleCount::x1,
			.usage = ImageUsage::SAMPLED | ImageUsage::STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Intermediate lightmap atlas",
			#endif
		});
	}

	ref<ImageView> Lightmapper::createIntermediateLightmapAtlasImageView() {
		return context.intermediateLightmapAtlasImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = imageAssetDescriptor->mipLevelCount,
			.layerOffset = 0,
			.layerCount = imageAssetDescriptor->layerCount,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Intermediate lightmap atlas view",
			#endif
		});
	}
}