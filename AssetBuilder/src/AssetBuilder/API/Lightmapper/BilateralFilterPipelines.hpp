// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<ComputePipeline> Lightmapper::createHorizontalBilateralFilterPipeline() const {
		return Renderer::createComputePipeline({
			.pipelineLayout = postProcessingPipelineLayout,
			.computeShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Horizontal bilateral filter compute",
					#endif
				},
				"Lightmapper/HorizontalBilateralFilter.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Horizontal bilateral filter",
			#endif
		});
	}

	ref<ComputePipeline> Lightmapper::createVerticalBilateralFilterPipeline() const {
		return Renderer::createComputePipeline({
			.pipelineLayout = postProcessingPipelineLayout,
			.computeShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Vertical bilateral filter compute",
					#endif
				},
				"Lightmapper/VerticalBilateralFilter.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertical bilateral filter",
			#endif
		});
	}
}