// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Sampler> Lightmapper::createIntermediateLightmapAtlasSampler() const {
		return device->createSampler({
			.magFilter = Filter::NEAREST,
			.minFilter = Filter::NEAREST,
			.addressModeU = AddressMode::CLAMP_TO_EDGE,
			.addressModeV = AddressMode::CLAMP_TO_EDGE,
			.addressModeW = AddressMode::CLAMP_TO_EDGE,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Intermediate lightmap atlas",
			#endif
		});
	}
}