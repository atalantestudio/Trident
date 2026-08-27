// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Debug/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> DebugRenderer::createGeometryPipeline() const {
		VertexInputBinding vertexInputBindings[] = {
			VertexInputBinding(InputRate::VERTEX),
		};

		vertexInputBindings[0].registerAttribute<svec3<float32>>(VertexInputAttributeFormat::R32G32B32_FLOAT);
		vertexInputBindings[0].registerAttribute<uint32>(VertexInputAttributeFormat::R10G10B10A2_UNORM);
		vertexInputBindings[0].registerAttribute<uint32>(VertexInputAttributeFormat::R10G10B10A2_UNORM);
		vertexInputBindings[0].registerAttribute<uint32>(VertexInputAttributeFormat::R16G16_FLOAT);

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
						.name = "Geometry vertex",
					#endif
				},
				"Debug/Geometry.vert"
			),
			.fragmentShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Geometry fragment",
					#endif
				},
				"Debug/Geometry.frag"
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
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Geometry",
			#endif
		});
	}
}