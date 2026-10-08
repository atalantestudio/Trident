// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Debug/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> DebugRenderer::createGeometryLinePipeline() const {
		VertexInputBinding vertexInputBindings[] = {
			VertexInputBinding(InputRate::VERTEX),
		};

		vertexInputBindings[0].registerAttribute<decltype(Vertex3D::position)>(VertexInputAttributeFormat::R32G32B32_FLOAT);
		vertexInputBindings[0].stride = sizeof(Vertex3D);

		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = false,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = geometryPipelineLayout,
			.vertexInputBindings = vertexInputBindings,
			.vertexShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Geometry line vertex",
					#endif
				},
				"Debug/GeometryLine.vert"
			),
			.fragmentShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Geometry line fragment",
					#endif
				},
				"Debug/GeometryLine.frag"
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
				.depthTest = true,
				.depthWrite = false,
				.compareOperation = CompareOperation::GREATER,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Geometry line",
			#endif
		});
	}
}