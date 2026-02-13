// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<RenderPipeline> VisibilityRenderer::createOutputPipeline() const {
		ShaderModuleDescriptor outputVertexShaderModuleDescriptor{};
		outputVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		outputVertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(outputVertexShaderModuleDescriptor, "Output vertex");

		ShaderModuleDescriptor outputFragmentShaderModuleDescriptor{};
		outputFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		outputFragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(outputFragmentShaderModuleDescriptor, "Output fragment");

		ColorBlendAttachment swapchainColorBlendAttachment{};
		swapchainColorBlendAttachment.blend = false;

		RenderPipelineDescriptor outputPipelineDescriptor{};
		outputPipelineDescriptor.pipelineLayout = outputPipelineLayout;
		outputPipelineDescriptor.vertexShaderModule = loadInternalShaderModule(outputVertexShaderModuleDescriptor, "Visibility/Output.vert");
		outputPipelineDescriptor.fragmentShaderModule = loadInternalShaderModule(outputFragmentShaderModuleDescriptor, "Visibility/Output.frag");
		outputPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		outputPipelineDescriptor.polygonMode = PolygonMode::FILL;
		outputPipelineDescriptor.cullMode = CullMode::BACK;
		outputPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		outputPipelineDescriptor.lineWidth = 1.0f;
		outputPipelineDescriptor.minSampleShading = 0.0f;
		outputPipelineDescriptor.colorBlendState.attachments = {swapchainColorBlendAttachment};
		outputPipelineDescriptor.depthStencilState.depthTest = false;
		outputPipelineDescriptor.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT;
		outputPipelineDescriptor.renderPass = presentRenderPass;
		outputPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(outputPipelineDescriptor, "Output");

		return createRenderPipeline(outputPipelineDescriptor);
	}
}