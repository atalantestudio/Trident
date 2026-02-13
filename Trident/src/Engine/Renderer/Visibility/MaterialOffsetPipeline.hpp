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

		ComputePipelineDescriptor materialOffsetPipelineDescriptor{};
		materialOffsetPipelineDescriptor.pipelineLayout = materialOffsetPipelineLayout;
		materialOffsetPipelineDescriptor.computeShaderModule = loadInternalShaderModule(materialOffsetShaderModuleDescriptor, "Visibility/MaterialOffset.comp");

		SET_DEBUG_NAME(materialOffsetPipelineDescriptor, "Material offset");

		return createComputePipeline(materialOffsetPipelineDescriptor);
	}
}