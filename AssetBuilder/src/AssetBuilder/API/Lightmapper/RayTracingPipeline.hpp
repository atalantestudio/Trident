// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<RayTracingPipeline> Lightmapper::createRayTracingPipeline() const {
		const ref<ShaderModule> shaderModules[] = {
			loadShaderModule(
				{
					.stage = ShaderStage::RAY_GENERATION,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Lightmap ray generation",
					#endif
				},
				"Lightmapper/Lightmap.rgen"
			),
			loadShaderModule(
				{
					.stage = ShaderStage::RAY_CLOSEST_HIT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Lightmap ray closest hit",
					#endif
				},
				"Lightmapper/Lightmap.rchit"
			),
			loadShaderModule(
				{
					.stage = ShaderStage::RAY_MISS,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Lightmap ray miss",
					#endif
				},
				"Lightmapper/Lightmap.rmiss"
			),
		};

		return device->createPipeline({
			.layout = rayTracingPipelineLayout,
			.shaderModules = shaderModules,
			.maxRayRecursionDepth = 3,
			.maxRayPayloadSize = 48,
			.maxRayAttributeSize = 8,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Ray tracing",
			#endif
		});
	}
}