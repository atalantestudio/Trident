// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	GraphicsPipelineDescriptor ForwardRenderer::getDynamicPipelineDescriptor(VertexInputBinding* vertexInputBindings) const {
		vertexInputBindings[0] = VertexInputBinding(InputRate::VERTEX);
		vertexInputBindings[0].registerAttribute<svec3<float32>>(VertexInputAttributeFormat::R32G32B32_FLOAT);
		vertexInputBindings[0].registerAttribute<uint32>(VertexInputAttributeFormat::R10G10B10A2_UNORM);
		vertexInputBindings[0].registerAttribute<uint32>(VertexInputAttributeFormat::R10G10B10A2_UNORM);
		vertexInputBindings[0].registerAttribute<uint32>(VertexInputAttributeFormat::R16G16_UNORM);

		vertexInputBindings[1] = VertexInputBinding(InputRate::INSTANCE);
		vertexInputBindings[1].registerAttribute<mat4x4<float32>>();
		vertexInputBindings[1].registerAttribute<mat3x3<float32>>();

		return {
			.vertexInputBindings = view<VertexInputBinding>(vertexInputBindings, 2),
			.vertexShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Forward vertex",
					#endif
				},
				"Forward/Forward.vert"
			),
			.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
			.polygonMode = PolygonMode::FILL,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.depthStencilState = {
				.depthTest = true,
				.depthWrite = true,
				.compareOperation = CompareOperation::GREATER,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = renderPass,
			.subpassIndex = 0,
		};
	}
}