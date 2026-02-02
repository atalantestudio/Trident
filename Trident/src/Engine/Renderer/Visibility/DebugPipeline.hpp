// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<RenderPipeline> VisibilityRenderer::createDebugPipeline() const {
		ShaderModuleDescriptor debugVertexShaderModuleDescriptor{};
		debugVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		debugVertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(debugVertexShaderModuleDescriptor, "Debug vertex");

		ShaderModuleDescriptor debugFragmentShaderModuleDescriptor{};
		debugFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		debugFragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(debugFragmentShaderModuleDescriptor, "Debug fragment");

		ColorBlendAttachment swapchainColorBlendAttachment{};
		swapchainColorBlendAttachment.blend = false;

		RenderPipelineDescriptor debugPipelineDescriptor{};
		debugPipelineDescriptor.pipelineLayout = debugPipelineLayout;
		debugPipelineDescriptor.vertexShaderModule = loadInternalShaderModule(debugVertexShaderModuleDescriptor, "Visibility/VisibilityDebug.vert");
		debugPipelineDescriptor.fragmentShaderModule = loadInternalShaderModule(debugFragmentShaderModuleDescriptor, "Visibility/VisibilityDebug.frag");
		debugPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		debugPipelineDescriptor.polygonMode = PolygonMode::FILL;
		debugPipelineDescriptor.cullMode = CullMode::BACK;
		debugPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		debugPipelineDescriptor.lineWidth = 1.0f;
		debugPipelineDescriptor.minSampleShading = 0.0f;
		debugPipelineDescriptor.colorBlendState.attachments = {swapchainColorBlendAttachment};
		debugPipelineDescriptor.depthStencilState.depthTest = false;
		debugPipelineDescriptor.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT;
		debugPipelineDescriptor.renderPass = renderPass;
		debugPipelineDescriptor.subpassIndex = DEBUG_SUBPASS_INDEX;

		SET_DEBUG_NAME(debugPipelineDescriptor, "Debug");

		return createRenderPipeline(debugPipelineDescriptor);
	}
}