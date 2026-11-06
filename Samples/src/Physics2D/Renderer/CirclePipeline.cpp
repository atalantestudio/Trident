// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> Physics2DRenderer::createCirclePipeline() const {
		VertexInputBinding vertexInputBindings[] = {
			VertexInputBinding(InputRate::VERTEX),
			VertexInputBinding(InputRate::INSTANCE),
		};

		vertexInputBindings[0].registerAttribute<vec2<float32>>();
		vertexInputBindings[1].registerAttribute<decltype(Transform2D::position)>();
		vertexInputBindings[1].stride = sizeof(Transform2D);

		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
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
						.name = "Circle vertex",
					#endif
				},
				"Physics2D/Circle.vert"
			),
			.fragmentShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Circle fragment",
					#endif
				},
				"Physics2D/Circle.frag"
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
				.depthTest = false,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Circle",
			#endif
		});
	}
}