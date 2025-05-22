#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLSwapchain::OpenGLSwapchain(const SwapchainDescriptor& descriptor) {
		construct(descriptor);
	}

	void OpenGLSwapchain::update(const SwapchainDescriptor& descriptor) {
		destroy();
		construct(descriptor);
	}

	uint32 OpenGLSwapchain::getExtentWidth() const {
		return extentWidth;
	}

	uint32 OpenGLSwapchain::getExtentHeight() const {
		return extentHeight;
	}

	void OpenGLSwapchain::construct(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		DEBUG_ASSERT(descriptor.extentWidth > 0 && descriptor.extentWidth < std::numeric_limits<uint32>::max());
		DEBUG_ASSERT(descriptor.extentHeight > 0 && descriptor.extentHeight < std::numeric_limits<uint32>::max());

		extentWidth = descriptor.extentWidth;
		extentHeight = descriptor.extentHeight;

		const OpenGLRenderPass& renderPass = *std::static_pointer_cast<OpenGLRenderPass>(descriptor.renderPass);

		constexpr uint32 imageCount = 1;

		images.resize(imageCount);
		imageViews.resize(imageCount);
		framebuffers.resize(imageCount);

		glCreateFramebuffers(imageCount, framebuffers.data());

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		imageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT;
		imageDescriptor.width = extentWidth;
		imageDescriptor.height = extentHeight;
		imageDescriptor.depth = 1;
		imageDescriptor.mipLevelCount = 1;
		imageDescriptor.arrayLayerCount = 1;
		imageDescriptor.sampleCount = 1;

		constexpr ImageViewDescriptor imageViewDescriptor;

		const uint32 attachmentCount = renderPass.getAttachmentCount();
		const uint32 colorAttachmentCount = renderPass.getColorAttachmentCount();
		const uint32 resolveAttachmentCount = renderPass.getResolveAttachmentCount();
		const uint32 presentAttachmentIndex = renderPass.getPresentAttachmentIndex();

		const chain<ref<ImageView>>& colorAttachmentImageViews = renderPass.getColorAttachmentImageViews();

		chain<ref<OpenGLImageView>> baseAttachmentsImageViews(attachmentCount);

		// Write color attachment image views (except for the presented attachment).
		for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < colorAttachmentCount; colorAttachmentIndex += 1) {
			if (resolveAttachmentCount == 0 && colorAttachmentIndex == presentAttachmentIndex) {
				continue;
			}

			const ref<ImageView>& colorAttachmentImageView = colorAttachmentImageViews[colorAttachmentIndex];

			baseAttachmentsImageViews[colorAttachmentIndex] = std::static_pointer_cast<OpenGLImageView>(colorAttachmentImageView);
		}

		// Write depth stencil attachment image view.
		if (renderPass.depthStencil) {
			const ref<ImageView>& depthStencilAttachmentImageView = renderPass.getDepthStencilAttachmentImageView();

			baseAttachmentsImageViews[colorAttachmentCount + resolveAttachmentCount] = std::static_pointer_cast<OpenGLImageView>(depthStencilAttachmentImageView);
		}

		for (uint32 imageIndex = 0; imageIndex < imageCount; imageIndex += 1) {
			images[imageIndex] = createScope<OpenGLImage>(imageDescriptor);
			imageViews[imageIndex] = createScope<OpenGLImageView>(*images.back(), imageViewDescriptor);

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[imageIndex]);

			chain<ref<OpenGLImageView>> attachmentImageViews(baseAttachmentsImageViews);

			// Override eligible attachments (all resolve and present) with a swapchain image view.
			if (resolveAttachmentCount > 0) {
				// Every resolve attachment renders to a swapchain image view.
				for (uint32 resolveAttachmentIndex = 0; resolveAttachmentIndex < resolveAttachmentCount; resolveAttachmentIndex += 1) {
					attachmentImageViews[colorAttachmentCount + resolveAttachmentIndex] = imageViews[imageIndex];
				}
			}
			else {
				// If there is no resolve attachments, then 1 color attachment must render to a swapchain image view.
				DEBUG_ASSERT(presentAttachmentIndex != std::numeric_limits<uint32>::max());

				attachmentImageViews[presentAttachmentIndex] = imageViews[imageIndex];
			}

			uint32 attachmentIndex = 0;

			for (uint32 i = 0; i < colorAttachmentCount + resolveAttachmentCount; i += 1) {
				if (i == presentAttachmentIndex) {
					continue;
				}

				const ref<OpenGLImageView>& imageView = attachmentImageViews[attachmentIndex];

				glFramebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0 + attachmentIndex,
					imageView->image.target,
					imageView->object,
					0
				);

				attachmentIndex += 1;
			}

			// TODO: Include depth stencil attachment.
			/*if (renderPass.depthStencil) {
				const ref<OpenGLImageView>& imageView = attachmentImageViews.back();

				glBindTexture(imageView->image.target, imageView->object);

				glFramebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_DEPTH_ATTACHMENT,
					imageView->image.target,
					imageView->object,
					0
				);
			}*/

			const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

			DEBUG_ASSERT(status == GL_FRAMEBUFFER_COMPLETE);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLSwapchain::destroy() {
		glDeleteFramebuffers(static_cast<uint32>(framebuffers.size()), framebuffers.data());

		imageViews.clear();
		images.clear();
		framebuffers.clear();
	}
}