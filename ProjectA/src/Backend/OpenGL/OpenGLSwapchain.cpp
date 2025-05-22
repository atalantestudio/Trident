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

		for (uint32 imageIndex = 0; imageIndex < imageCount; imageIndex += 1) {
			images[imageIndex] = createScope<OpenGLImage>(imageDescriptor);
			imageViews[imageIndex] = createScope<OpenGLImageView>(*images.back(), imageViewDescriptor);

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[imageIndex]);

			for (uint32 attachmentIndex = 0; attachmentIndex < renderPass.colorAttachments.size(); attachmentIndex += 1) {
				const OpenGLImageView* imageView;

				//if (attachmentImageViews[attachmentIndex] == nullptr) {
					imageView = imageViews[imageIndex].get();
				/*}
				else {
					imageView = std::static_pointer_cast<OpenGLImageView>(attachmentImageViews[attachmentIndex]).get();
				}*/

				glBindTexture(imageView->image.target, imageView->image.object);

				glFramebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0,
					imageView->image.target,
					imageView->image.object,
					0
				);
			}

			/*if (renderPass.depthStencil) {
				const OpenGLImageView& imageView = *std::static_pointer_cast<OpenGLImageView>(attachmentImageViews.back());

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, imageView.image.target, imageView.image.object, 0);
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