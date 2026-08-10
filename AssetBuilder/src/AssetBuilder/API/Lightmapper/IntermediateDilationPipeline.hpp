// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<ComputePipeline> Lightmapper::createIntermediateDilationPipeline() const {
		return device->createComputePipeline({
			.pipelineLayout = postProcessingPipelineLayout,
			.computeShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Intermediate dilation compute",
					#endif
				},
				"Lightmapper/IntermediateDilation.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Intermediate dilation",
			#endif
		});
	}
}