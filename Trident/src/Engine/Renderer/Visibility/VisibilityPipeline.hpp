// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> VisibilityRenderer::createVisibilityPipeline() const {
		const ShaderModuleDescriptor visibilityVertexShaderModuleDescriptor = {
			.stage = ShaderStage::VERTEX,
			.entryPoint = "main",
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Visibility vertex",
			#endif
		};

		ref<ShaderModule> vertexShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				vertexShaderModule = loadInternalShaderModule(visibilityVertexShaderModuleDescriptor, "Visibility/Visibility.vert");

				break;
			}
			case GraphicsAPI::GL: {
				vertexShaderModule = loadInternalShaderModule(visibilityVertexShaderModuleDescriptor, "Visibility/VisibilitySingleSet.vert");

				break;
			}
		}

		const ref<ShaderModule> fragmentShaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::FRAGMENT,
				.entryPoint = "main",
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Visibility fragment",
				#endif
			},
			"Visibility/Visibility.frag"
		);

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = visibilityPipelineLayout,
			.vertexShaderModule = vertexShaderModule,
			.fragmentShaderModule = fragmentShaderModule,
			.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
			.polygonMode = PolygonMode::FILL,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = colorBlendAttachments,
			},
			.depthStencilState = {
				.depthTest = true,
				.depthWrite = true,
				.compareOperation = CompareOperation::LESS,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = visibilityRenderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Visibility",
			#endif
		});
	}
}