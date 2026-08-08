// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<GraphicsPipeline> Lightmapper::createGBufferPipeline() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<uint32>();

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
			{
				.blend = false,
			},
			{
				.blend = false,
			},
		};

		return Renderer::createGraphicsPipeline({
			.pipelineLayout = gBufferPipelineLayout,
			.vertexInputBindings = vertexInputBinding,
			.vertexShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "G-Buffer vertex",
					#endif
				},
				"Lightmapper/GBuffer.vert"
			),
			.fragmentShaderModule = Renderer::loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "G-Buffer fragment",
					#endif
				},
				"Lightmapper/GBuffer.frag"
			),
			.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
			.polygonMode = PolygonMode::FILL,
			.cullMode = CullMode::NONE,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = colorBlendAttachments,
			},
			.depthStencilState = {
				.depthTest = false,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = gBufferRenderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer",
			#endif
		});
	}
}