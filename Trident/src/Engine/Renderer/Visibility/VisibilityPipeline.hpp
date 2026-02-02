// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<RenderPipeline> VisibilityRenderer::createVisibilityPipeline() const {
		ShaderModuleDescriptor visibilityVertexShaderModuleDescriptor{};
		visibilityVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		visibilityVertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(visibilityVertexShaderModuleDescriptor, "Visibility vertex");

		ShaderModuleDescriptor visibilityFragmentShaderModuleDescriptor{};
		visibilityFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		visibilityFragmentShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(visibilityFragmentShaderModuleDescriptor, "Visibility fragment");

		#ifdef GL_SOURCE_FROM_SPIRV
			if (Renderer::getGraphicsApi() == GraphicsAPI::GL) {
				// TODO: Create a pair of SPIR-V shaders compatible with the OpenGL adapter.
				ENGINE_WARNING("The Visibility.*.spv shaders are not compatible with the OpenGL adapter.");
			}
		#endif

		ColorBlendAttachment visibilityColorBlendAttachment{};
		visibilityColorBlendAttachment.blend = false;

		RenderPipelineDescriptor visibilityPipelineDescriptor{};
		visibilityPipelineDescriptor.pipelineLayout = visibilityPipelineLayout;
		visibilityPipelineDescriptor.vertexShaderModule = loadInternalShaderModule(visibilityVertexShaderModuleDescriptor, "Visibility/Visibility.vert");
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
		visibilityPipelineDescriptor.renderPass = renderPass;
		visibilityPipelineDescriptor.subpassIndex = VISIBILITY_SUBPASS_INDEX;

		SET_DEBUG_NAME(visibilityPipelineDescriptor, "Visibility");

		return createRenderPipeline(visibilityPipelineDescriptor);
	}
}