// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> VisibilityRenderer::createOutputPipeline() const {
		const ref<ShaderModule> vertexShaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::VERTEX,
				.entryPoint = "main",
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Output vertex",
				#endif
			},
			"Visibility/Output.vert"
		);

		const ref<ShaderModule> fragmentShaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::FRAGMENT,
				.entryPoint = "main",
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Output fragment",
				#endif
			},
			"Visibility/Output.frag"
		);

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = outputPipelineLayout,
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
				.depthTest = false,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = presentRenderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Output",
			#endif
		});
	}
}