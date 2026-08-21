// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../../Physics2D/Renderer/Physics2DRenderer.hpp"

namespace Trident {
	#if defined(SHOW_DISTANCE)
		ref<GraphicsPipeline> Physics2DRenderer::createDistancePipeline() const {
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
							.name = "Distance vertex",
						#endif
					},
					"Physics2D/Distance.vert"
				),
				.fragmentShaderModule = loadShaderModule(
					{
						.stage = ShaderStage::FRAGMENT,
						.entryPoint = "main",
						#if defined(DEBUG_OBJECT_NAME)
							.name = "Distance fragment",
						#endif
					},
					"Physics2D/Distance.frag"
				),
				.primitiveTopology = PrimitiveTopology::LINE_LIST,
				.polygonMode = PolygonMode::LINE,
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
				.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
				.renderPass = renderPass,
				.subpassIndex = 0,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Distance",
				#endif
			});
		}
	#endif
}