// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<ComputePipeline> Lightmapper::createTraceRaysPipeline() const {
		return device->createComputePipeline({
			.pipelineLayout = traceRaysPipelineLayout,
			.computeShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Trace rays compute",
					#endif
				},
				"Lightmapper/TraceRays.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Trace rays",
			#endif
		});
	}
}