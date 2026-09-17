// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> ForwardRenderer::createDirectionalShadowMapPipeline() const {
		VertexInputBinding vertexInputBindings[] = {
			VertexInputBinding(InputRate::VERTEX),
			VertexInputBinding(InputRate::INSTANCE),
		};

		vertexInputBindings[0].registerAttribute<svec3<float32>>(VertexInputAttributeFormat::R32G32B32_FLOAT);
		vertexInputBindings[0].stride = sizeof(Vertex3D);
		vertexInputBindings[1].registerAttribute<mat4x4<float32>>();
		vertexInputBindings[1].stride = sizeof(Instance3D);

		return createGraphicsPipeline({
			.pipelineLayout = directionalShadowMapPipelineLayout,
			.vertexInputBindings = vertexInputBindings,
			.vertexShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Directional shadow map vertex",
					#endif
				},
				"Forward/DirectionalShadowMap.vert"
			),
			.fragmentShaderModule = nullptr,
			.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
			.polygonMode = PolygonMode::FILL,

			// TODO(renderer/forward/directional-shadow-map): Cull front faces.
			.cullMode = CullMode::NONE,

			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = {},
			},
			.depthStencilState = {
				.depthTest = true,
				.depthWrite = true,
				.compareOperation = CompareOperation::LESS,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = directionalShadowMapRenderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Directional shadow map",
			#endif
		});
	}
}