// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Window/Layer/RenderPassLayer.hpp"
#include "Engine/API/Window/LayerGroup/RenderPassLayerGroup.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	RenderPassLayerGroup::RenderPassLayerGroup(range<uint32, uint32> layerRange) :
		LayerGroup(layerRange)
	{}

	void RenderPassLayerGroup::initialize() {
		renderPass = Renderer::createRenderPass(getRenderPassDescriptor());
		clearValues = getClearValues();

		for (const ref<Layer> layer : layers) {
			ref<RenderPassLayer> renderPassLayer = *pointer_cast<const ref<RenderPassLayer>*>(&layer);
			renderPassLayer->renderPass = renderPass;
		}

		const ImageFormat surfaceFormat = Renderer::getSurface()->getFormat();

		uint32 attachmentIndex = 0;
		uint32 imageCount = 0;
		uint32 imageViewCount = 0;

		while (attachmentIndex < renderPass->getAttachmentCount()) {
			const Attachment& attachment = renderPass->attachments[attachmentIndex];

			if (attachment.format == surfaceFormat && attachment.finalLayout == ImageLayout::PRESENT_SRC) {
				swapchainAttachmentIndex = attachmentIndex;
				swapchainImageIndex = attachmentIndex;

				if (attachment.sampleCount > 1) {
					swapchainImageIndex = renderPass->getAttachmentCount();

					imageViewCount += 1;
				}
			}
			else {
				imageCount += 1;
			}

			imageViewCount += 1;
			attachmentIndex += 1;
		}

		ENGINE_ASSERT(swapchainAttachmentIndex != max<uint32>(), "The render pass must have a presentable attachment.");

		framebufferImages = sequence<ref<Image>>(imageCount);
		framebufferImageViews = sequence<ref<ImageView>>(imageViewCount);

		createFramebufferImages();
		createFramebuffers();
	}

	void RenderPassLayerGroup::createFramebufferImages() {
		const Extent2D extent = Renderer::getSwapchain()->getExtent();
		const view<Subpass> subpasses = renderPass->getSubpasses();

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		// TODO(window/layer-group/render-pass): Add support for multiple subpasses.
		ENGINE_ASSERT(subpasses.count() == 1, "Multiple subpasses aren't supported.");

		uint32 subpassIndex = 0;

		while (subpassIndex < subpasses.count()) {
			const Subpass& subpass = subpasses[subpassIndex];

			uint32 colorAttachmentReferenceIndex = 0;

			while (colorAttachmentReferenceIndex < subpass.colorAttachmentReferences.count()) {
				const AttachmentReference colorAttachmentReference = subpass.colorAttachmentReferences[colorAttachmentReferenceIndex];

				if (colorAttachmentReference.index == swapchainAttachmentIndex || colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
					colorAttachmentReferenceIndex += 1;

					continue;
				}

				const uint32 imageIndex = getImageIndex(colorAttachmentReference.index);

				#if defined(DEBUG_OBJECT_NAME)
					name = Logger::format("Color attachment []", colorAttachmentReference.index);
				#endif

				framebufferImages[imageIndex] = renderPass->createColorAttachmentImage(
					{
						.type = ImageType::TEXTURE_2D,
						.extent = {
							.width = extent.width,
							.height = extent.height,
							.depth = 1,
						},
						.mipLevelCount = 1,
						.layerCount = 1,

						// TODO(window/layer-group/render-pass): Add support for additional usage.

						#if defined(DEBUG_OBJECT_NAME)
							.name = name,
						#endif
					},
					colorAttachmentReference.index
				);

				#if defined(DEBUG_OBJECT_NAME)
					name = Logger::format("Color attachment [] view", colorAttachmentReference.index);
				#endif

				createFramebufferImageView(imageIndex, colorAttachmentReference.index, name);

				colorAttachmentReferenceIndex += 1;
			}

			if (subpass.depthStencilAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
				const uint32 imageIndex = getImageIndex(subpass.depthStencilAttachmentReference.index);

				framebufferImages[imageIndex] = renderPass->createDepthStencilAttachmentImage(
					{
						.type = ImageType::TEXTURE_2D,
						.extent = {
							.width = extent.width,
							.height = extent.height,
							.depth = 1,
						},
						.mipLevelCount = 1,
						.layerCount = 1,

						// TODO(window/layer-group/render-pass): Add support for additional usage.

						#if defined(DEBUG_OBJECT_NAME)
							.name = "Depth stencil",
						#endif
					},
					subpassIndex
				);

				#if defined(DEBUG_OBJECT_NAME)
					name = "Depth stencil view";
				#endif

				createFramebufferImageView(imageIndex, subpass.depthStencilAttachmentReference.index, name);
			}

			subpassIndex += 1;
		}
	}

	void RenderPassLayerGroup::createFramebuffers() {
		const ref<Swapchain> swapchain = Renderer::getSwapchain();

		framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

		FramebufferDescriptor framebufferDescriptor = {
			.renderPass = renderPass,
			.imageViews = framebufferImageViews,
			.extent = swapchain->getExtent(),
		};

		// TODO(graphics/framebuffer): Use a view to represent the framebuffer's image views.
		for (ref<ImageView>& imageView : framebufferDescriptor.imageViews) {
			imageView.owning = false;
		}

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 framebufferIndex = 0;

		while (framebufferIndex < framebuffers.count()) {
			#if defined(DEBUG_OBJECT_NAME)
				name = Logger::format("In flight []", framebufferIndex);

				framebufferDescriptor.name = name;
			#endif

			framebufferDescriptor.imageViews[swapchainImageIndex] = swapchain->getImageView(framebufferIndex);

			framebuffers[framebufferIndex] = Renderer::createFramebuffer(framebufferDescriptor);

			framebufferIndex += 1;
		}
	}

	void RenderPassLayerGroup::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {
		commandEncoder->beginRenderPass({
			.framebuffer = framebuffers[Renderer::getSwapchainImageIndex()],
			.clearValues = clearValues,
		});

		for (const ref<Layer> layer : layers) {
			layer->preRender();
			layer->render(commandEncoder, remainingFrameTime);
			layer->postRender();
		}

		commandEncoder->endRenderPass();
	}

	void RenderPassLayerGroup::resize() {
		createFramebufferImages();
		createFramebuffers();
	}

	uint32 RenderPassLayerGroup::getImageIndex(uint32 attachmentIndex) const {
		if (swapchainAttachmentIndex < attachmentIndex) {
			return attachmentIndex - 1;
		}

		return attachmentIndex;
	}

	void RenderPassLayerGroup::createFramebufferImageView(uint32 imageIndex, uint32 attachmentIndex, view<char8> name) {
		framebufferImageViews[attachmentIndex] = framebufferImages[imageIndex]->createView({
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
			#if defined(DEBUG_OBJECT_NAME)
				.name = name,
			#endif
		});
	}
}