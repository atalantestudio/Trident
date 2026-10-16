// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
		ref<GraphicsPipeline> ForwardRenderer::createGIShadingPipeline() const {
			VertexInputBinding vertexInputBinding(InputRate::VERTEX);
			vertexInputBinding.registerAttribute<decltype(GIVertex::position)>();
			vertexInputBinding.registerAttribute<decltype(GIVertex::a)>();
			vertexInputBinding.registerAttribute<decltype(GIVertex::b)>();
			vertexInputBinding.registerAttribute<decltype(GIVertex::c)>();

			const ColorBlendAttachment colorBlendAttachments[] = {
				{
					.blend = true,
					.srcColorBlendFactor = BlendFactor::ONE,
					.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
				},
			};

			return createGraphicsPipeline({
				.pipelineLayout = giShadingPipelineLayout,
				.vertexInputBindings = vertexInputBinding,
				.vertexShaderModule = loadInternalShaderModule(
					{
						.stage = ShaderStage::VERTEX,
						.entryPoint = "main",
						#if defined(DEBUG_OBJECT_NAME)
							.name = "GI shading vertex",
						#endif
					},
					"Forward/GI/Shading.vert"
				),
				.fragmentShaderModule = loadInternalShaderModule(
					{
						.stage = ShaderStage::FRAGMENT,
						.entryPoint = "main",
						#if defined(DEBUG_OBJECT_NAME)
							.name = "GI shading fragment",
						#endif
					},
					"Forward/GI/Shading.frag"
				),
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
					// TODO(forward/gi): Re-enable depth testing.
					.depthTest = false,
				},
				.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
				.renderPass = renderPass,
				.subpassIndex = 0,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "GI shading",
				#endif
			});
		}
	#endif
}