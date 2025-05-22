#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLSwapchain::OpenGLSwapchain(const SwapchainDescriptor& descriptor) {
		construct(descriptor);
	}

	void OpenGLSwapchain::recreate(const SwapchainDescriptor& descriptor) {
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

		GLint maxColorAttachments;

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);

		DEBUG_ASSERT(renderPass.colorAttachmentCount <= static_cast<uint32>(maxColorAttachments));

		constexpr uint32 imageCount = 1;

		framebuffers.resize(imageCount);

		glCreateFramebuffers(static_cast<uint32>(framebuffers.size()), framebuffers.data());

		constexpr ImageViewDescriptor imageViewDescriptor;

		for (uint32 imageIndex = 0; imageIndex < imageCount; imageIndex += 1) {
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[imageIndex]);

			for (uint32 attachmentIndex = 0; attachmentIndex < renderPass.attachments.size(); attachmentIndex += 1) {
				const Attachment& attachment = renderPass.attachments[attachmentIndex];

				ImageDescriptor imageDescriptor{};
				imageDescriptor.type = ImageType::TEXTURE_2D;
				imageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
				imageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT;
				imageDescriptor.width = extentWidth;
				imageDescriptor.height = extentHeight;
				imageDescriptor.depth = 1;
				imageDescriptor.mipLevelCount = 1;
				imageDescriptor.arrayLayerCount = 1;
				imageDescriptor.sampleCount = attachment.sampleCount;

				images.emplace_back(createScope<OpenGLImage>(imageDescriptor));

				imageViews.emplace_back(createScope<OpenGLImageView>(*images.back(), imageViewDescriptor));

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, images.back()->target, images.back()->object, 0);
			}

			DEBUG_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLSwapchain::destroy() {
		glDeleteFramebuffers(static_cast<uint32>(framebuffers.size()), framebuffers.data());

		fchain<GLuint> glImages(imageViews.size() + images.size());

		uint64 imageIndex = 0;

		for (; imageIndex < imageViews.size(); imageIndex += 1) {
			glImages[imageIndex] = imageViews[imageIndex]->object;
		}

		for (; imageIndex < images.size(); imageIndex += 1) {
			glImages[imageIndex] = images[imageIndex]->object;
		}

		glDeleteTextures(static_cast<uint32>(glImages.count), &glImages[0]);

		framebuffers.clear();
		images.clear();
		imageViews.clear();
	}

	bool OpenGLSwapchain::requiresResolvePass() const {
		for (const scope<OpenGLImageView>& imageView : imageViews) {
			if (imageView->image.target == GL_TEXTURE_2D_MULTISAMPLE) {
				return true;
			}
		}

		return false;
	}
}