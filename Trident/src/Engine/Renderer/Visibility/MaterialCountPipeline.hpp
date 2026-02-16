// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<ComputePipeline> VisibilityRenderer::createMaterialCountPipeline() const {
		ShaderModuleDescriptor materialCountShaderModuleDescriptor{};
		materialCountShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		materialCountShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(materialCountShaderModuleDescriptor, "Material count");

		ComputePipelineDescriptor materialCountPipelineDescriptor{};
		materialCountPipelineDescriptor.pipelineLayout = materialCountPipelineLayout;
		materialCountPipelineDescriptor.computeShaderModule = loadInternalShaderModule(materialCountShaderModuleDescriptor, "Visibility/MaterialCount.comp");

		SET_DEBUG_NAME(materialCountPipelineDescriptor, "Material count");

		return createComputePipeline(materialCountPipelineDescriptor);
	}
}