// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Sampler> ForwardRenderer::createMaterialSampler() const {
		return createSampler({
			.magFilter = Filter::LINEAR,
			.minFilter = Filter::LINEAR,
			.addressModeU = AddressMode::REPEAT,
			.addressModeV = AddressMode::REPEAT,
			.addressModeW = AddressMode::REPEAT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Material",
			#endif
		});
	}
}