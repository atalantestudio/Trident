// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Sampler> VisibilityRenderer::createMaterialImageSampler() const {
		SamplerDescriptor materialImageSamplerDescriptor{};
		materialImageSamplerDescriptor.magFilter = SamplerFilter::NEAREST;
		materialImageSamplerDescriptor.minFilter = SamplerFilter::NEAREST;

		return createSampler(materialImageSamplerDescriptor);
	}
}