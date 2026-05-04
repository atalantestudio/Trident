// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Sampler> ForwardRenderer::createImageSampler() const {
		return createSampler({
			.magFilter = SamplerFilter::LINEAR,
			.minFilter = SamplerFilter::LINEAR,
		});
	}
}