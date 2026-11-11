// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	ref<GraphicsPipeline> Physics2DRenderer::createShapeFillPipeline() const {
		VertexInputBinding vertexInputBindings[] = {
			VertexInputBinding(InputRate::VERTEX),
			VertexInputBinding(InputRate::INSTANCE),
		};

		vertexInputBindings[0].registerAttribute<vec2<float32>>();
		vertexInputBindings[1].registerAttribute<decltype(PhysicsRendererInstance2D::position)>();
		vertexInputBindings[1].registerAttribute<decltype(PhysicsRendererInstance2D::centroid)>();
		vertexInputBindings[1].registerAttribute<decltype(PhysicsRendererInstance2D::rotation)>();
		vertexInputBindings[1].registerAttribute<decltype(PhysicsRendererInstance2D::color)>();
		vertexInputBindings[1].registerAttribute<decltype(PhysicsRendererInstance2D::shapeId)>();

		// TODO(physics/2d/renderer): Restore depth and fill/stroke colors.
		// vertexInputBindings[1].registerAttribute<decltype(RigidBody::fillColor)>();
		// vertexInputBindings[1].registerAttribute<decltype(RigidBody::strokeColor)>();
		// vertexInputBindings[1].registerAttribute<decltype(RigidBody::depth)>();

		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = false,
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
						.name = "Shape fill vertex",
					#endif
				},
				"Physics2D/ShapeFill.vert"
			),
			.fragmentShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Shape fill fragment",
					#endif
				},
				"Physics2D/FragmentRGB.frag"
			),
			.primitiveTopology = PrimitiveTopology::TRIANGLE_STRIP,
			.polygonMode = PolygonMode::FILL,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = colorBlendAttachments,
			},
			.depthStencilState = {
				.depthTest = false,

				// TODO(physics/2d/renderer): Restore depth testing.
				// .depthWrite = true,
				// .compareOperation = CompareOperation::GREATER_OR_EQUAL,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Shape fill",
			#endif
		});
	}
}