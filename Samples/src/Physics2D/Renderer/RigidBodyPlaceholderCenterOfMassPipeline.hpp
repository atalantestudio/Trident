// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "../../Physics2D/Renderer/Physics2DRenderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> Physics2DRenderer::createRigidBodyPlaceholderCenterOfMassPipeline() const {
		VertexInputBinding vertexInputBindings[] = {
			VertexInputBinding(InputRate::VERTEX),
		};

		vertexInputBindings[0].registerAttribute<svec2<float32>>();
		vertexInputBindings[0].stride = sizeof(CenterOfMass);

		const ShaderModuleDescriptor vertexShaderModuleDescriptor = {
			.stage = ShaderStage::VERTEX,
			.entryPoint = "main",
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Placeholder center of mass vertex",
			#endif
		};

		ref<ShaderModule> vertexShaderModule;

		switch (getGraphicsApi()) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderCenterOfMass.vert");

				break;
			}
			case GraphicsAPI::GL: {
				vertexShaderModule = loadShaderModule(vertexShaderModuleDescriptor, "Physics2D/PlaceholderCenterOfMassSingleSet.vert");

				break;
			}
		}

		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = false,
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
						.name = "Center of mass fragment",
					#endif
				},
				"Physics2D/CenterOfMass.frag"
			),
			.primitiveTopology = PrimitiveTopology::POINT_LIST,
			.polygonMode = PolygonMode::POINT,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 1.0f,
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
				.name = "Rigid body placeholder center of mass",
			#endif
		});
	}
}