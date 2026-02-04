// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	void VisibilityRenderer::createMaterialPipelines() {
		materialPipelines = sequence<ref<ComputePipeline>>(scene->materials.count());

		ShaderModuleDescriptor materialShaderModuleDescriptor{};
		materialShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		materialShaderModuleDescriptor.entryPoint = "main";

		ComputePipelineDescriptor materialPipelineDescriptor{};

		#if defined(BUILD_MODE_DEBUG)
			sequence<char8> name;
		#endif

		uint32 materialPipelineIndex = 0;

		while (materialPipelineIndex < materialPipelines.count()) {
			#if defined(BUILD_MODE_DEBUG)
				name = Logger::format("Material []", materialPipelineIndex);

				materialShaderModuleDescriptor.name = name;
				materialPipelineDescriptor.name = name;
			#endif

			materialPipelineDescriptor.pipelineLayout = materialPipelineLayouts[materialPipelineIndex];
			materialPipelineDescriptor.computeShaderModule = loadInternalShaderModule(materialShaderModuleDescriptor, scene->materials[materialPipelineIndex].shaderModulePath);

			materialPipelines[materialPipelineIndex] = createComputePipeline(materialPipelineDescriptor);

			materialPipelineIndex += 1;
		}
	}
}