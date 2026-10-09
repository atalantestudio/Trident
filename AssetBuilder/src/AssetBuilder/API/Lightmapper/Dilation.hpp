// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<RenderPass> Lightmapper::createDilationRenderPass() const {
		return device->createRenderPass({
			.attachments = {
				// Lightmap atlas
				{
					.format = imageAssetDescriptor->format,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::TRANSFER_SRC_OPTIMAL,
				},
			},
			.subpasses = {
				{
					.colorAttachmentReferences = {
						{
							.index = 0,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
				},
			},
		});
	}

	ref<Framebuffer> Lightmapper::createDilationFramebuffer() const {
		return device->createFramebuffer({
			.renderPass = dilationRenderPass,
			.imageViews = {context.lightmapAtlasImageView},
			.extent = {imageAssetDescriptor->extent.x, imageAssetDescriptor->extent.y},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Dilation",
			#endif
		});
	}

	ref<GraphicsPipeline> Lightmapper::createDilationPipeline() const {
		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = false,
			},
		};

		return device->createGraphicsPipeline({
			.pipelineLayout = dilationPipelineLayout,
			.vertexInputBindings = {},
			.vertexShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Dilation vertex",
					#endif
				},
				"Lightmapper/Dilation.vert"
			),
			.fragmentShaderModule = loadShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Dilation fragment",
					#endif
				},
				"Lightmapper/Dilation.frag"
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
				.depthTest = false,
			},
			.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR,
			.renderPass = dilationRenderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Dilation",
			#endif
		});
	}
}