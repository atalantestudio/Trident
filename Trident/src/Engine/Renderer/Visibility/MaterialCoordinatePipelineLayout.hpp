// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createMaterialCoordinatePipelineLayout() const {
		PipelineLayoutDescriptor materialCoordinatePipelineLayoutDescriptor{};
		materialCoordinatePipelineLayoutDescriptor.descriptorSetLayouts = {materialCoordinateDescriptorSetLayout};

		SET_DEBUG_NAME(materialCoordinatePipelineLayoutDescriptor, "Material coordinate");

		return createPipelineLayout(materialCoordinatePipelineLayoutDescriptor);
	}
}