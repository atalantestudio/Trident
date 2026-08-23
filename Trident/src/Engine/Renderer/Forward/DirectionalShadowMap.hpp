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
		const Extent3D extent = directionalShadowMapImage->getExtent();

		directionalShadowMapFramebuffers = sequence<ref<Framebuffer>>(getSwapchain()->getImageCount());

		FramebufferDescriptor framebufferDescriptor = {
			.renderPass = renderPass,
			.imageViews = {directionalShadowMapImageView},
			.extent = {extent.width, extent.height},
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

	ref<Image> ForwardRenderer::createDirectionalShadowMapImage() const {
		return directionalShadowMapRenderPass->createDepthStencilAttachmentImage(
			{
				.type = ImageType::TEXTURE_2D,
				.extent = {1024, 1024, 1},
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