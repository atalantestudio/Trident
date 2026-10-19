// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Debug/Renderer.hpp"

namespace Trident {
	ref<Image> DebugRenderer::createSkyboxImage(ref<ImageAsset> imageAsset) const {
		return createDynamicImage({
			.type = ImageType::TYPE_2D,
			.flags = ImageFlags::CUBE_COMPATIBLE,
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = imageAsset->getExtent(),
			.mipLevelCount = 1,
			.layerCount = 6,
			.sampleCount = ImageSampleCount::x1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Skybox",
			#endif
		});
	}
}