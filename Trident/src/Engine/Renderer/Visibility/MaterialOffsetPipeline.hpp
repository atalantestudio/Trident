// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<ComputePipeline> VisibilityRenderer::createMaterialOffsetPipeline() const {
		const ref<ShaderModule> shaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::COMPUTE,
				.entryPoint = "main",
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Material offset compute",
				#endif
			},
			"Visibility/MaterialOffset.comp"
		);

		return createComputePipeline({
			.pipelineLayout = materialOffsetPipelineLayout,
			.computeShaderModule = shaderModule,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Material offset",
			#endif
		});
	}
}