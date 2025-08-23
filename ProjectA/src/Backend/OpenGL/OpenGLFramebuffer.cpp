#include "pch.hpp"

#include "Backend/OpenGL/OpenGLFramebuffer.hpp"
#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"

#include "Backend/OpenGL/ImageFormat.hpp"

namespace ProjectA {
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDescriptor& descriptor) :
		Framebuffer(descriptor)
	{
		#ifdef OPENGL_USE_DSA
			glCreateFramebuffers(1, &object);
		#else
			glGenFramebuffers(1, &object);
			glBindFramebuffer(GL_FRAMEBUFFER, object);
		#endif

		// All images attached to a framebuffer must have the same number of samples.
		// Use this information to find if the framebuffer contains multisampled attachments or not.
		bool resolve = false;

		for (uint32 imageViewIndex = 0; imageViewIndex < descriptor.imageViews.count; imageViewIndex += 1) {
			const ref<ImageView>& imageView = descriptor.imageViews[imageViewIndex];

			if (imageView->getImage()->getSampleCount() > 1) {
				resolve = true;

				break;
			}
		}

		uint32 boundAttachmentIndex = 0;

		for (uint32 imageViewIndex = 0; imageViewIndex < descriptor.imageViews.count; imageViewIndex += 1) {
			const ref<ImageView>& imageView = descriptor.imageViews[imageViewIndex];
			const OpenGLImage* glImage = static_cast<const OpenGLImage*>(imageView->getImage());
			const OpenGLImageView& glImageView = *std::static_pointer_cast<OpenGLImageView>(imageView);

			if (resolve && imageView->getImage()->getSampleCount() == 1) {
				continue;
			}

			GLenum attachment;

			const ImageFormat internalFormat = fromGlInternalFormat(glImage->internalFormat);

			// TODO: Handle stencil-only internal formats with GL_STENCIL_ATTACHMENT.
			switch (internalFormat) {
				case ImageFormat::D32_SFLOAT:
					attachment = GL_DEPTH_ATTACHMENT;

					break;
				case ImageFormat::D32_SFLOAT_S8_UINT:
				case ImageFormat::D24_UNORM_S8_UINT:
					attachment = GL_DEPTH_STENCIL_ATTACHMENT;

					break;
				default:
					attachment = GL_COLOR_ATTACHMENT0 + boundAttachmentIndex;

					break;
			}

			// TODO: Move that switch out of the loop. The function to call must be known beforehand (maybe even before creating the image).
			switch (glImage->target) {
				case GL_TEXTURE_2D:

				// TODO: Check if it's right.
				case GL_TEXTURE_2D_MULTISAMPLE:

				case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
					#ifdef OPENGL_USE_DSA
						glNamedFramebufferTexture(
							object,
							attachment,
							glImageView.object,
							imageView->getMipLevelOffset()
						);
					#else
						glFramebufferTexture2D(
							GL_FRAMEBUFFER,
							attachment,
							glImage->target,
							glImageView.object,
							imageView->getMipLevelOffset()
						);
					#endif

					break;
				default:
					ENGINE_ERROR("Unhandled image target ([]).", glImage->target);
					DEBUG_ASSERT(false);

					break;
			}

			boundAttachmentIndex += 1;
		}

		#ifdef OPENGL_USE_DSA
			sequence<GLenum> colorAttachments(colorAttachmentCount);

			for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < colorAttachmentCount; colorAttachmentIndex += 1) {
				colorAttachments[colorAttachmentIndex] = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
			}

			glNamedFramebufferDrawBuffers(object, colorAttachmentCount, colorAttachments);
		#endif

		#ifdef BUILD_MODE_DEBUG
			GLenum status;

			#ifdef OPENGL_USE_DSA
				status = glCheckNamedFramebufferStatus(object, GL_FRAMEBUFFER);
			#else
				status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			#endif

			ENGINE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete.");
		#endif
	}

	OpenGLFramebuffer::~OpenGLFramebuffer() {
		glDeleteFramebuffers(1, &object);
	}
}