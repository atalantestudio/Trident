#include "pch.hpp"

#include "Backend/OpenGL/OpenGLFramebuffer.hpp"
#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"

#include "Backend/OpenGL/ImageFormat.hpp"

namespace ProjectA {
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDescriptor& descriptor) :
		Framebuffer(descriptor)
	{
		glCreateFramebuffers(1, &object);
		glBindFramebuffer(GL_FRAMEBUFFER, object);

		for (uint32 imageViewIndex = 0; imageViewIndex < descriptor.imageViews.count; imageViewIndex += 1) {
			const ref<ImageView>& imageView = descriptor.imageViews[imageViewIndex];
			const OpenGLImage* glImage = static_cast<const OpenGLImage*>(imageView->getImage());
			const OpenGLImageView& glImageView = *std::static_pointer_cast<OpenGLImageView>(imageView);

			GLenum attachment;

			if (isDepthFormat(glImage->internalFormat)) {
				attachment = GL_DEPTH_ATTACHMENT;
			}
			else {
				attachment = GL_COLOR_ATTACHMENT0 + imageViewIndex;
			}

			// TODO: Move that switch out of the loop. The function to call must be known beforehand (maybe even when creating the image).
			switch (glImage->target) {
				case GL_TEXTURE_2D:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
					glFramebufferTexture2D(
						GL_FRAMEBUFFER,
						attachment,
						glImage->target,
						glImageView.object,
						imageView->getMipLevelOffset()
					);

					break;
				default:
					ENGINE_ERROR("Unhandled image target ([]).", glImage->target);
					DEBUG_ASSERT(false);

					break;
			}
		}

		#ifdef OPENGL_USE_DSA
			sequence<GLenum> colorAttachments(colorAttachmentCount);

			for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < colorAttachmentCount; colorAttachmentIndex += 1) {
				colorAttachments[colorAttachmentIndex] = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
			}

			glNamedFramebufferDrawBuffers(object, colorAttachmentCount, colorAttachments);
		#endif

		#ifdef BUILD_MODE_DEBUG
			const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

			ENGINE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "The bound framebuffer is incomplete.");
		#endif

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer() {
		glDeleteFramebuffers(1, &object);
	}
}