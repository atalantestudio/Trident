#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLSwapchain::OpenGLSwapchain(const SwapchainDescriptor& descriptor) {
		construct(descriptor);
	}

	uint32 OpenGLSwapchain::getImageCount() const {
		return imageCount;
	}

	// TODO: Fix memory leak when resizing on the depth test and multisample demos.
	void OpenGLSwapchain::getImages(const ref<RenderPass>& renderPass) {
		const ref<OpenGLRenderPass>& glRenderPass = std::static_pointer_cast<OpenGLRenderPass>(renderPass);

		if (renderPass->present && renderPass->getColorAttachmentCount() == 1 && !renderPass->resolve && !renderPass->depthStencil) {
			return;
		}

		images.resize(imageCount);
		imageViews.resize(imageCount);
		framebuffers.resize(imageCount);

		glCreateFramebuffers(imageCount, framebuffers.data());

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		imageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT;
		imageDescriptor.width = extentWidth;
		imageDescriptor.height = extentHeight;
		imageDescriptor.depth = 1;
		imageDescriptor.mipLevelCount = 1;
		imageDescriptor.arrayLayerCount = 1;
		imageDescriptor.sampleCount = 1;

		constexpr ImageViewDescriptor imageViewDescriptor;

		const uint32 attachmentCount = renderPass->getAttachmentCount();
		const uint32 colorAttachmentCount = renderPass->getColorAttachmentCount();
		const uint32 resolveAttachmentCount = renderPass->getResolveAttachmentCount();
		const uint32 presentAttachmentIndex = renderPass->getPresentAttachmentIndex();

		const std::vector<ref<ImageView>>& colorAttachmentImageViews = renderPass->getColorAttachmentImageViews();

		std::vector<ref<OpenGLImageView>> baseAttachmentsImageViews(attachmentCount);

		// Write color attachment image views (except for the presented attachment).
		for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < colorAttachmentCount; colorAttachmentIndex += 1) {
			if (resolveAttachmentCount == 0 && colorAttachmentIndex == presentAttachmentIndex) {
				continue;
			}

			const ref<ImageView>& colorAttachmentImageView = colorAttachmentImageViews[colorAttachmentIndex];

			baseAttachmentsImageViews[colorAttachmentIndex] = std::static_pointer_cast<OpenGLImageView>(colorAttachmentImageView);
		}

		// Write depth stencil attachment image view.
		if (renderPass->depthStencil) {
			const ref<ImageView>& depthStencilAttachmentImageView = renderPass->getDepthStencilAttachmentImageView();

			baseAttachmentsImageViews[colorAttachmentCount + resolveAttachmentCount] = std::static_pointer_cast<OpenGLImageView>(depthStencilAttachmentImageView);
		}

		for (uint32 imageIndex = 0; imageIndex < imageCount; imageIndex += 1) {
			images[imageIndex] = createRef<OpenGLImage>(imageDescriptor);
			imageViews[imageIndex] = createRef<OpenGLImageView>(*images[imageIndex], imageViewDescriptor);

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[imageIndex]);

			std::vector<OpenGLImageView*> attachmentImageViews(baseAttachmentsImageViews.size());

			// Populate attachmentImageViews.
			for (uint32 attachmentImageViewIndex = 0; attachmentImageViewIndex < attachmentImageViews.size(); attachmentImageViewIndex += 1) {
				attachmentImageViews[attachmentImageViewIndex] = baseAttachmentsImageViews[attachmentImageViewIndex].get();
			}

			// Override eligible attachments (all resolve and present) with a swapchain image view.
			if (resolveAttachmentCount > 0) {
				// Every resolve attachment renders to a swapchain image view.
				for (uint32 resolveAttachmentIndex = 0; resolveAttachmentIndex < resolveAttachmentCount; resolveAttachmentIndex += 1) {
					attachmentImageViews[colorAttachmentCount + resolveAttachmentIndex] = imageViews[imageIndex].get();
				}
			}
			else {
				// If there is no resolve attachments, then 1 color attachment must render to a swapchain image view.
				DEBUG_ASSERT(presentAttachmentIndex != std::numeric_limits<uint32>::max());

				attachmentImageViews[presentAttachmentIndex] = imageViews[imageIndex].get();
			}

			uint32 boundAttachmentIndex = 0;

			for (uint32 attachmentIndex = resolveAttachmentCount; attachmentIndex < colorAttachmentCount + resolveAttachmentCount; attachmentIndex += 1) {
				if (resolveAttachmentCount > 0 && attachmentIndex == presentAttachmentIndex) {
					continue;
				}

				const OpenGLImageView* imageView = attachmentImageViews[boundAttachmentIndex];

				glFramebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0 + boundAttachmentIndex,
					imageView->image.target,
					imageView->object,
					0
				);

				boundAttachmentIndex += 1;
			}

			if (renderPass->depthStencil) {
				const OpenGLImageView* imageView = attachmentImageViews.back();

				// TODO: Use GL_DEPTH_STENCIL_ATTACHMENT instead of GL_DEPTH_ATTACHMENT.
				glFramebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_DEPTH_ATTACHMENT,
					imageView->image.target,
					imageView->object,
					0
				);
			}

			const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

			DEBUG_ASSERT(status == GL_FRAMEBUFFER_COMPLETE);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLSwapchain::update(const SwapchainDescriptor& descriptor) {
		destroy();
		construct(descriptor);
	}

	void OpenGLSwapchain::construct(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		DEBUG_ASSERT(descriptor.extentWidth > 0);
		DEBUG_ASSERT(descriptor.extentHeight > 0);

		extentWidth = descriptor.extentWidth;
		extentHeight = descriptor.extentHeight;
	}

	void OpenGLSwapchain::destroy() {
		glDeleteFramebuffers(static_cast<uint32>(framebuffers.size()), framebuffers.data());

		imageViews.clear();
		images.clear();
		framebuffers.clear();
	}
}