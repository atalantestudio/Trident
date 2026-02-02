// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createDebugPipelineLayout() const {
		PipelineLayoutDescriptor debugPipelineLayoutDescriptor{};
		debugPipelineLayoutDescriptor.descriptorSetLayouts = {
			debugDescriptorSetLayout,
			clusterDescriptorSetLayout,
			instanceDescriptorSetLayout,
		};

		SET_DEBUG_NAME(debugPipelineLayoutDescriptor, "Debug");

		return createPipelineLayout(debugPipelineLayoutDescriptor);
	}
}