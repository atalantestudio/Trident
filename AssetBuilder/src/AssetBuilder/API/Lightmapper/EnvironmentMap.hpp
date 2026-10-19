// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Image> Lightmapper::createEnvironmentMapImage(Extent3D extent) const {
		// TODO(asset-builder/lightmapper): Use a static memory allocation.
		return device->createDynamicImage({
			.type = ImageType::TYPE_2D,
			.flags = ImageFlags::CUBE_COMPATIBLE,
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = extent,
			.mipLevelCount = 1,
			.layerCount = 6,
			.sampleCount = ImageSampleCount::x1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Environment map",
			#endif
		});
	}

	ref<ImageView> Lightmapper::createEnvironmentMapImageView() const {
		return environmentMapImage->createView({
			.type = ImageViewType::TYPE_CUBE,
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 6,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Environment map view",
			#endif
		});
	}
}