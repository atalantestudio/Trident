// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	GraphicsPipelineDescriptor ForwardRenderer::getStaticPipelineDescriptor() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX, false);
		vertexInputBinding.registerAttribute<svec3<uint32>>();

		VertexInputBinding lightmapCoordinateInputBinding(InputRate::VERTEX, false);
		lightmapCoordinateInputBinding.registerAttribute<svec2<float32>>();

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE, false);
		instanceInputBinding.registerAttribute<mat4x4<float32>>();
		instanceInputBinding.registerAttribute<mat3x3<float32>>();

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		return {
			.vertexInputBindings = {vertexInputBinding, lightmapCoordinateInputBinding, instanceInputBinding},
			.vertexShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Static geometry vertex",
					#endif
				},
				"Forward/StaticGeometry/StaticGeometry.vert"
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
		};
	}
}