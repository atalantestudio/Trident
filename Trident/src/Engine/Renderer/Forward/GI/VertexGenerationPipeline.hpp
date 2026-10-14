// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
		ref<ComputePipeline> ForwardRenderer::createGIVertexGenerationPipeline() const {
			return createComputePipeline({
				.pipelineLayout = giVertexGenerationPipelineLayout,
				.computeShaderModule = loadInternalShaderModule(
					{
						.stage = ShaderStage::COMPUTE,
						.entryPoint = "main",
						#if defined(DEBUG_OBJECT_NAME)
							.name = "GI vertex generation compute",
						#endif
					},
					"Forward/GI/VertexGeneration.comp"
				),
				#if defined(DEBUG_OBJECT_NAME)
					.name = "GI vertex generation",
				#endif
			});
		}
	#endif
}