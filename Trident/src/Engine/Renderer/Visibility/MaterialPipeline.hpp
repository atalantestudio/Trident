// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	void VisibilityRenderer::createMaterialPipelines() {
		materialPipelines = sequence<ref<ComputePipeline>>(scene->materialPipelineDescriptors.count());

		uint32 materialPipelineIndex = 0;

		ComputePipelineDescriptor materialPipelineDescriptor{};

		while (materialPipelineIndex < materialPipelines.count()) {
			// TODO: Create material pipelines.

			materialPipelineIndex += 1;
		}
	}
}