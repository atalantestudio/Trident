// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../../Physics2D/Renderer/Physics2DRenderer.hpp"

namespace Trident {
	#if defined(SHOW_FEATURES)
		ref<GraphicsPipeline> Physics2DRenderer::createVertexFeaturePipeline() const {
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
							.name = "Vertex feature vertex",
						#endif
					},
					"Physics2D/VertexFeature.vert"
				),
				.fragmentShaderModule = loadShaderModule(
					{
						.stage = ShaderStage::FRAGMENT,
						.entryPoint = "main",
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Vertex feature fragment",
						#endif
					},
					"Physics2D/VertexFeature.frag"
				),
				.primitiveTopology = PrimitiveTopology::POINT_LIST,
				.polygonMode = PolygonMode::POINT,
				.cullMode = CullMode::BACK,
				.frontFace = FrontFace::CLOCKWISE,
				.lineWidth = 1.0f,
				.minSampleShading = 1.0f,
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
					.name = "Vertex feature",
				#endif
			});
		}
	#endif
}