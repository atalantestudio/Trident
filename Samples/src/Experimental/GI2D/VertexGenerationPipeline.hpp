// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Experimental/GI2D/Renderer.hpp"

namespace Trident {
	ref<ComputePipeline> GI2DExperimentalRenderer::createVertexGenerationPipeline() const {
		return createComputePipeline({
			.pipelineLayout = pipelineLayout,
			.computeShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertex generation compute",
					#endif
				},
				"Experimental/GI2D/VertexGeneration.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex generation",
			#endif
		});
	}
}