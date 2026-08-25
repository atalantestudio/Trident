// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Image> ForwardRenderer::createEnvironmentMapImage() const {
		return createDynamicImage({
			.type = ImageType::TYPE_2D,
			.flags = ImageFlags::CUBE_COMPATIBLE,
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {2048, 2048, 1},
			.mipLevelCount = 1,
			.layerCount = 6,
			.sampleCount = 1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Environment map",
			#endif
		});
	}

	ref<ImageView> ForwardRenderer::createEnvironmentMapImageView() const {
		return environmentMapImage->createView({
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