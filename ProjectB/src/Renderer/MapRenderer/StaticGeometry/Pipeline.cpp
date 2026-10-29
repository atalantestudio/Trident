// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	ref<GraphicsPipeline> MapRenderer::createStaticGeometryPipeline() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<decltype(Vertex3D::position)>(VertexInputAttributeFormat::R32G32B32_FLOAT);
		vertexInputBinding.registerAttribute<decltype(Vertex3D::normal)>(VertexInputAttributeFormat::R10G10B10A2_UNORM);
		vertexInputBinding.stride = sizeof(Vertex3D);

		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = false,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = staticGeometryPipelineLayout,
			.vertexInputBindings = vertexInputBinding,
			.vertexShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Static geometry vertex",
					#endif
				},
				"Map/StaticGeometry.vert"
			),
			.fragmentShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Geometry fill fragment",
					#endif
				},
				"Map/StaticGeometry.frag"
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
				.depthTest = true,
				.depthWrite = true,
				.compareOperation = CompareOperation::GREATER,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static geometry",
			#endif
		});
	}
}