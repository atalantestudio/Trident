// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<ComputePipeline> VisibilityRenderer::createMaterialCountPipeline() const {
		const ShaderModuleDescriptor shaderModuleDescriptor = {
			.stage = ShaderStage::COMPUTE,
			.entryPoint = "main",
			#if defined(BUILD_MODE_DEBUG)
				.name = "Material count compute",
			#endif
		};

		ref<ShaderModule> shaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				shaderModule = loadInternalShaderModule(shaderModuleDescriptor, "Visibility/MaterialCount.comp");

				break;
			}
			case GraphicsAPI::GL: {
				shaderModule = loadInternalShaderModule(shaderModuleDescriptor, "Visibility/MaterialCountGL.comp");

				break;
			}
		}

		return createComputePipeline({
			.pipelineLayout = materialCountPipelineLayout,
			.computeShaderModule = shaderModule,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Material count",
			#endif
		});
	}
}