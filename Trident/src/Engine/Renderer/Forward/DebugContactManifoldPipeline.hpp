// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> ForwardRenderer::createDebugContactManifoldPipeline() const {
		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec3<float32>>();

		const ref<ShaderModule> vertexShaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::VERTEX,
				.entryPoint = "main",
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Debug contact manifold vertex",
				#endif
			},
			"Forward/DebugContactManifold.vert"
		);

		const ref<ShaderModule> fragmentShaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::FRAGMENT,
				.entryPoint = "main",
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Debug contact manifold fragment",
				#endif
			},
			"Forward/DebugContactManifold.frag"
		);

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = debugContactManifoldPipelineLayout,
			.vertexInputBindings = {vertexInputBinding},
			.vertexShaderModule = vertexShaderModule,
			.fragmentShaderModule = fragmentShaderModule,
			.primitiveTopology = PrimitiveTopology::LINE_STRIP,
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
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Debug contact manifold",
			#endif
		});
	}
}