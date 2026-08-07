// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	GraphicsPipelineDescriptor ForwardRenderer::getDynamicPipelineDescriptor() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX, false);
		vertexInputBinding.registerAttribute<svec3<float32>>(VertexInputAttributeFormat::R32G32B32_FLOAT);
		vertexInputBinding.registerAttribute<uint32>(VertexInputAttributeFormat::R10G10B10A2_UNORM);
		vertexInputBinding.registerAttribute<uint32>(VertexInputAttributeFormat::R10G10B10A2_UNORM);
		vertexInputBinding.registerAttribute<uint32>(VertexInputAttributeFormat::R16G16_UNORM);

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE, false);
		instanceInputBinding.registerAttribute<mat4x4<float32>>();
		instanceInputBinding.registerAttribute<mat3x3<float32>>();

		return {
			.vertexInputBindings = {vertexInputBinding, instanceInputBinding},
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