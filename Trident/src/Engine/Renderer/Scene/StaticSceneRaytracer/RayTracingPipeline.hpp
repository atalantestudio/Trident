// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/StaticSceneRayTracer/StaticSceneRayTracer.hpp"

namespace Trident {
	ref<ComputePipeline> StaticSceneRayTracer::createRayTracingPipeline() const {
		return createComputePipeline({
			.pipelineLayout = rayTracingPipelineLayout,
			.computeShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::COMPUTE,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Trace compute",
					#endif
				},
				"Scene/Trace.comp"
			),
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Ray tracing",
			#endif
		});
	}
}