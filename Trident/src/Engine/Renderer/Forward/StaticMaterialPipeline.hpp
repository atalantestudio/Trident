// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> ForwardRenderer::createStaticMaterialPipeline() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec3<float32>>();

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = staticMaterialPipelineLayout,
			.vertexInputBindings = {vertexInputBinding},
			.vertexShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Static material vertex",
					#endif
				},
				"Forward/StaticMaterial.vert"
			),
			.fragmentShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Static material fragment",
					#endif
				},
				"Forward/StaticMaterial.frag"
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
				.compareOperation = CompareOperation::LESS,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static material",
			#endif
		});
	}
}