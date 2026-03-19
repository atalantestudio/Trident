// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> VisibilityRenderer::createVisibilityPipeline() const {
		ShaderModuleDescriptor visibilityVertexShaderModuleDescriptor{};
		visibilityVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		visibilityVertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(visibilityVertexShaderModuleDescriptor, "Visibility vertex");

		ShaderModuleDescriptor visibilityFragmentShaderModuleDescriptor{};
		visibilityFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		visibilityFragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(visibilityFragmentShaderModuleDescriptor, "Visibility fragment");

		ColorBlendAttachment visibilityColorBlendAttachment{};
		visibilityColorBlendAttachment.blend = false;

		GraphicsPipelineDescriptor visibilityPipelineDescriptor{};
		visibilityPipelineDescriptor.pipelineLayout = visibilityPipelineLayout;

		switch (getGraphicsApi()) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				visibilityPipelineDescriptor.vertexShaderModule = loadInternalShaderModule(visibilityVertexShaderModuleDescriptor, "Visibility/Visibility.vert");

				break;
			}
			case GraphicsAPI::GL: {
				visibilityPipelineDescriptor.vertexShaderModule = loadInternalShaderModule(visibilityVertexShaderModuleDescriptor, "Visibility/VisibilitySingleSet.vert");

				break;
			}
		}

		visibilityPipelineDescriptor.fragmentShaderModule = loadInternalShaderModule(visibilityFragmentShaderModuleDescriptor, "Visibility/Visibility.frag");
		visibilityPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		visibilityPipelineDescriptor.polygonMode = PolygonMode::FILL;
		visibilityPipelineDescriptor.cullMode = CullMode::BACK;
		visibilityPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		visibilityPipelineDescriptor.lineWidth = 1.0f;
		visibilityPipelineDescriptor.minSampleShading = 0.0f;
		visibilityPipelineDescriptor.colorBlendState.attachments = {visibilityColorBlendAttachment};
		visibilityPipelineDescriptor.depthStencilState.depthTest = true;
		visibilityPipelineDescriptor.depthStencilState.depthWrite = true;
		visibilityPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::LESS;
		visibilityPipelineDescriptor.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT;
		visibilityPipelineDescriptor.renderPass = visibilityRenderPass;
		visibilityPipelineDescriptor.subpassIndex = 0;

		SET_DEBUG_NAME(visibilityPipelineDescriptor, "Visibility");

		return createGraphicsPipeline(visibilityPipelineDescriptor);
	}
}