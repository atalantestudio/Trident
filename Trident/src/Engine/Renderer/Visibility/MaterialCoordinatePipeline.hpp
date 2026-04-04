// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<ComputePipeline> VisibilityRenderer::createMaterialCoordinatePipeline() const {
		const ShaderModuleDescriptor shaderModuleDescriptor = {
			.stage = ShaderStage::COMPUTE,
			.entryPoint = "main",
			#if defined(BUILD_MODE_DEBUG)
				.name = "Material coordinate compute",
			#endif
		};

		ref<ShaderModule> shaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				shaderModule = loadInternalShaderModule(shaderModuleDescriptor, "Visibility/MaterialCoordinate.comp");

				break;
			}
			case GraphicsAPI::GL: {
				shaderModule = loadInternalShaderModule(shaderModuleDescriptor, "Visibility/MaterialCoordinateGL.comp");

				break;
			}
		}

		return createComputePipeline({
			.pipelineLayout = materialCoordinatePipelineLayout,
			.computeShaderModule = shaderModule,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Material coordinate",
			#endif
		});
	}
}