// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createOutputPipelineLayout() const {
		PipelineLayoutDescriptor outputPipelineLayoutDescriptor{};
		outputPipelineLayoutDescriptor.descriptorSetLayouts = {outputDescriptorSetLayout};

		SET_DEBUG_NAME(outputPipelineLayoutDescriptor, "Output");

		return createPipelineLayout(outputPipelineLayoutDescriptor);
	}
}