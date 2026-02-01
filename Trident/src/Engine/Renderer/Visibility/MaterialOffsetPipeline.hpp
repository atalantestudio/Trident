// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<ComputePipeline> VisibilityRenderer::createMaterialOffsetPipeline() const {
		ShaderModuleDescriptor materialOffsetShaderModuleDescriptor{};
		materialOffsetShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		materialOffsetShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(materialOffsetShaderModuleDescriptor, "Material offset");

		const ref<ShaderModule> materialOffsetShaderModule = loadInternalShaderModule(materialOffsetShaderModuleDescriptor, "Visibility/MaterialOffset.comp");

		ComputePipelineDescriptor materialOffsetComputePipelineDescriptor{};
		materialOffsetComputePipelineDescriptor.pipelineLayout = materialOffsetPipelineLayout;
		materialOffsetComputePipelineDescriptor.computeShaderModule = materialOffsetShaderModule;

		SET_DEBUG_NAME(materialOffsetComputePipelineDescriptor, "Material offset");

		return createComputePipeline(materialOffsetComputePipelineDescriptor);
	}
}