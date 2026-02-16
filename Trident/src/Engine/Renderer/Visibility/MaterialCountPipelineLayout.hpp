// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createMaterialCountPipelineLayout() const {
		PipelineLayoutDescriptor materialCountPipelineLayoutDescriptor{};
		materialCountPipelineLayoutDescriptor.descriptorSetLayouts = {materialCountDescriptorSetLayout};

		SET_DEBUG_NAME(materialCountPipelineLayoutDescriptor, "Material count");

		return createPipelineLayout(materialCountPipelineLayoutDescriptor);
	}
}