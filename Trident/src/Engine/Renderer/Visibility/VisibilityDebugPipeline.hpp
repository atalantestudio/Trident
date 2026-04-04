// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
		ref<ComputePipeline> VisibilityRenderer::createVisibilityDebugPipeline() const {
			const ShaderModuleDescriptor shaderModuleDescriptor = {
				.stage = ShaderStage::COMPUTE,
				.entryPoint = "main",
				#if defined(BUILD_MODE_DEBUG)
					.name = "Visibility debug compute",
				#endif
			};

			ref<ShaderModule> shaderModule;

			switch (getGraphicsApi()) {
				case GraphicsAPI::DX12:
				case GraphicsAPI::VK: {
					shaderModule = loadInternalShaderModule(shaderModuleDescriptor, "Visibility/VisibilityDebug.comp");

					break;
				}
				case GraphicsAPI::GL: {
					shaderModule = loadInternalShaderModule(shaderModuleDescriptor, "Visibility/VisibilityDebugGL.comp");

					break;
				}
			}

			return createComputePipeline({
				.pipelineLayout = visibilityDebugPipelineLayout,
				.computeShaderModule = shaderModule,
				#if defined(BUILD_MODE_DEBUG)
					.name = "Visibility debug",
				#endif
			});
		}
	#endif
}