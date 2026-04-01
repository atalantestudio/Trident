// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<ComputePipeline> VisibilityRenderer::createMaterialCoordinatePipeline() const {
		const ref<ShaderModule> shaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::COMPUTE,
				.entryPoint = "main",
				#if defined(BUILD_MODE_DEBUG)
					.name = "Material coordinate",
				#endif
			},
			"Visibility/MaterialCoordinate.comp"
		);

		return createComputePipeline({
			.pipelineLayout = materialCoordinatePipelineLayout,
			.computeShaderModule = shaderModule,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Material coordinate",
			#endif
		});
	}
}