// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Sampler> VisibilityRenderer::createMaterialShaderImageSampler() const {
		return createSampler({
			.magFilter = SamplerFilter::LINEAR,
			.minFilter = SamplerFilter::LINEAR,
		});
	}
}