// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<RenderPass> ForwardRenderer::createDirectionalShadowMapRenderPass() const {
		return Renderer::createRenderPass({
			.attachments = {
				{
					.format = ImageFormat::D32_SFLOAT,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,

					// TODO(renderer/forward/directional-shadow-map): Use UNDEFINED instead?
					.storeOperation = StoreOperation::STORE,

					.finalLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				},
			},
			.subpasses = {
				{
					.depthStencilAttachmentReference = {
						.index = 0,
						.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					},
				},
			},
		});
	}

	void ForwardRenderer::createDirectionalShadowMapFramebuffers() {
		directionalShadowMapFramebuffers = sequence<ref<Framebuffer>>(getSwapchain()->getImageCount());

		FramebufferDescriptor framebufferDescriptor = {
			.renderPass = directionalShadowMapRenderPass,
			.imageViews = {directionalShadowMapImageView},
			.extent = DIRECTIONAL_SHADOW_MAP_EXTENT,
		};

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 framebufferIndex = 0;

		while (framebufferIndex < directionalShadowMapFramebuffers.count()) {
			#if defined(DEBUG_OBJECT_NAME)
				name = Logger::format("Directional shadow map []", framebufferIndex);

				framebufferDescriptor.name = name;
			#endif

			directionalShadowMapFramebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);

			framebufferIndex += 1;
		}
	}

	ref<DescriptorSetLayout> ForwardRenderer::createDirectionalShadowMapDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Directional light
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::VERTEX,
			},
		};

		return createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Directional shadow map",
			#endif
		});
	}

	ref<PipelineLayout> ForwardRenderer::createDirectionalShadowMapPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {directionalShadowMapDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Directional shadow map",
			#endif
		});
	}

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
				.compareOperation = CompareOperation::GREATER,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = directionalShadowMapRenderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Directional shadow map",
			#endif
		});
	}

	ref<Image> ForwardRenderer::createDirectionalShadowMapImage() const {
		return directionalShadowMapRenderPass->createDepthStencilAttachmentImage(
			{
				.type = ImageType::TYPE_2D,
				.extent = {DIRECTIONAL_SHADOW_MAP_EXTENT.width, DIRECTIONAL_SHADOW_MAP_EXTENT.height, 1},
				.mipLevelCount = 1,
				.layerCount = 1,
				.additionalUsage = ImageUsage::SAMPLED,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Directional shadow map",
				#endif
			},
			0
		);
	}

	ref<ImageView> ForwardRenderer::createDirectionalShadowMapImageView() const {
		return directionalShadowMapImage->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Directional shadow map view",
			#endif
		});
	}
}