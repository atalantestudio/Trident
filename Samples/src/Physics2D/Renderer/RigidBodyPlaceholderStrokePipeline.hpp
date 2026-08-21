// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../../Physics2D/Renderer/Physics2DRenderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> Physics2DRenderer::createRigidBodyPlaceholderStrokePipeline() const {
		VertexInputBinding vertexInputBindings[] = {
			VertexInputBinding(InputRate::VERTEX),
			VertexInputBinding(InputRate::INSTANCE),
		};

		vertexInputBindings[0].registerAttribute<svec2<float32>>();
		vertexInputBindings[1].registerAttribute<mat3x3<float32>>();
		vertexInputBindings[1].stride = sizeof(RigidBody);

		const ShaderModuleDescriptor vertexShaderModuleDescriptor = {
			.stage = ShaderStage::VERTEX,
			.entryPoint = "main",
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Placeholder stroke vertex",
			#endif
		};

		ref<ShaderModule> vertexShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderStroke.vert");

				break;
			}
			case GraphicsAPI::GL: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderStrokeSingleSet.vert");

				break;
			}
		}

		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = true,
				.srcColorBlendFactor = BlendFactor::SRC_ALPHA,
				.dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = placeholderPipelineLayout,
			.vertexInputBindings = vertexInputBindings,
			.vertexShaderModule = vertexShaderModule,
			.fragmentShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Placeholder stroke fragment",
					#endif
				},
				"Physics2D/PlaceholderStroke.frag"
			),
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
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Rigid body placeholder stroke",
			#endif
		});
	}
}