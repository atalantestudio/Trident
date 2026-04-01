// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Sampler> VisibilityRenderer::createMaterialImageSampler() const {
		return createSampler({
			.magFilter = SamplerFilter::NEAREST,
			.minFilter = SamplerFilter::NEAREST,
		});
	}
}