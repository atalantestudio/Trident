// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> ForwardRenderer::createDebugHullWireframePipeline() const {
		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat4x4<float32>>();
		instanceInputBinding.registerAttribute<mat3x3<float32>>();

		const ref<ShaderModule> vertexShaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::VERTEX,
				.entryPoint = "main",
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Debug hull wireframe vertex",
				#endif
			},
			"Forward/DebugHullWireframe.vert"
		);

		const ref<ShaderModule> fragmentShaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::FRAGMENT,
				.entryPoint = "main",
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Debug hull wireframe fragment",
				#endif
			},
			"Forward/DebugHullWireframe.frag"
		);

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = debugHullWireframePipelineLayout,
			.vertexInputBindings = {instanceInputBinding},
			.vertexShaderModule = vertexShaderModule,
			.fragmentShaderModule = fragmentShaderModule,
			.primitiveTopology = PrimitiveTopology::LINE_LIST,
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
				.compareOperation = CompareOperation::LESS_OR_EQUAL,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Debug hull wireframe",
			#endif
		});
	}
}