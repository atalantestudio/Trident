// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Sampler> ForwardRenderer::createDirectionalShadowMapSampler() const {
		return createSampler({
			.magFilter = Filter::NEAREST,
			.minFilter = Filter::NEAREST,
			.addressModeU = AddressMode::CLAMP_TO_EDGE,
			.addressModeV = AddressMode::CLAMP_TO_EDGE,
			.addressModeW = AddressMode::CLAMP_TO_EDGE,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Directional shadow map",
			#endif
		});
	}
}