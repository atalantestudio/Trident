// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createMaterialOffsetPipelineLayout() const {
		PipelineLayoutDescriptor materialOffsetPipelineLayoutDescriptor{};
		materialOffsetPipelineLayoutDescriptor.descriptorSetLayouts = {materialOffsetDescriptorSetLayout};

		SET_DEBUG_NAME(materialOffsetPipelineLayoutDescriptor, "Material offset");

		return createPipelineLayout(materialOffsetPipelineLayoutDescriptor);
	}
}