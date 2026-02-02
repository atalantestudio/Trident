// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<ComputePipeline> VisibilityRenderer::createMaterialCoordinatePipeline() const {
		ShaderModuleDescriptor materialCoordinateShaderModuleDescriptor{};
		materialCoordinateShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		materialCoordinateShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(materialCoordinateShaderModuleDescriptor, "Material coordinate");

		ComputePipelineDescriptor materialCoordinatePipelineDescriptor{};
		materialCoordinatePipelineDescriptor.pipelineLayout = materialCoordinatePipelineLayout;
		materialCoordinatePipelineDescriptor.computeShaderModule = loadInternalShaderModule(materialCoordinateShaderModuleDescriptor, "Visibility/MaterialCoordinate.comp");

		SET_DEBUG_NAME(materialCoordinatePipelineDescriptor, "Material coordinate");

		return createComputePipeline(materialCoordinatePipelineDescriptor);
	}
}