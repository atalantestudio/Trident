// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../../Physics2D/Renderer/Physics2DRenderer.hpp"

namespace Trident {
	#if defined(SHOW_FEATURES)
		ref<GraphicsPipeline> Physics2DRenderer::createEdgeFeaturePipeline() const {
			VertexInputBinding vertexInputBindings[] = {
				VertexInputBinding(InputRate::VERTEX),
			};

			vertexInputBindings[0].registerAttribute<svec2<float32>>();

			const ColorBlendAttachment colorBlendAttachments[] = {
				{
					.blend = false,
				},
			};

			return createGraphicsPipeline({
				.pipelineLayout = cameraViewPipelineLayout,
				.vertexInputBindings = vertexInputBindings,
				.vertexShaderModule = loadShaderModule(
					{
						.stage = ShaderStage::VERTEX,
						.entryPoint = "main",
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Edge feature vertex",
						#endif
					},
					"Physics2D/EdgeFeature.vert"
				),
				.fragmentShaderModule = loadShaderModule(
					{
						.stage = ShaderStage::FRAGMENT,
						.entryPoint = "main",
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Edge feature fragment",
						#endif
					},
					"Physics2D/EdgeFeature.frag"
				),
				.primitiveTopology = PrimitiveTopology::LINE_LIST,
				.polygonMode = PolygonMode::LINE,
				.cullMode = CullMode::BACK,
				.frontFace = FrontFace::CLOCKWISE,
				.lineWidth = 3.0f,
				.minSampleShading = 0.0f,
				.colorBlendState = {
					.attachments = colorBlendAttachments,
				},
				.depthStencilState = {
					.depthTest = false,
				},
				.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
				.renderPass = renderPass,
				.subpassIndex = 0,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Edge feature",
				#endif
			});
		}
	#endif
}