// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createVisibilityPipelineLayout() const {
		PipelineLayoutDescriptor visibilityPipelineLayoutDescriptor{};
		visibilityPipelineLayoutDescriptor.descriptorSetLayouts = {
			cameraDescriptorSetLayout,
			clusterDescriptorSetLayout,
			instanceDescriptorSetLayout,
			visibilityMaterialCountDescriptorSetLayout,
		};

		SET_DEBUG_NAME(visibilityPipelineLayoutDescriptor, "Visibility");

		return createPipelineLayout(visibilityPipelineLayoutDescriptor);
	}
}