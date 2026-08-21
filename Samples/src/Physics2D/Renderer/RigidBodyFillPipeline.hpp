// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../../Physics2D/Renderer/Physics2DRenderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> Physics2DRenderer::createRigidBodyFillPipeline() const {
		VertexInputBinding vertexInputBindings[] = {
			VertexInputBinding(InputRate::VERTEX),
			VertexInputBinding(InputRate::INSTANCE),
		};

		vertexInputBindings[0].registerAttribute<svec2<float32>>();
		vertexInputBindings[1].registerAttribute<mat3x3<float32>>();
		vertexInputBindings[1].registerAttribute<vec4<float32>>();
		vertexInputBindings[1].registerAttribute<vec4<float32>>();
		vertexInputBindings[1].registerAttribute<float32>();

		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = cameraViewPipelineLayout,
			.vertexInputBindings = vertexInputBindings,
			.vertexShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Rigid body fill vertex",
					#endif
				},
				"Physics2D/RigidBodyFill.vert"
			),
			.fragmentShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Rigid body fill fragment",
					#endif
				},
				"Physics2D/FragmentRGBA.frag"
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
				.compareOperation = CompareOperation::GREATER_OR_EQUAL,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Rigid body fill",
			#endif
		});
	}
}