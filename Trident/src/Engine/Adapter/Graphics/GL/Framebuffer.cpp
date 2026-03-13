// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Framebuffer::Framebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor) :
		Trident::Framebuffer(device, descriptor)
	{}

	ref<Trident::Framebuffer> GL::createFramebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Framebuffer> framebuffer = createRef<GL::Framebuffer>(device, descriptor);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateFramebuffers(1, &framebuffer->framebuffer);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
			glGenFramebuffers(1, &framebuffer->framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		// All images attached to a framebuffer must have the same number of samples.
		// Use this information to find if the framebuffer contains multisampled attachments or not.
		bool resolve = false;

		for (uint32 imageViewIndex = 0; imageViewIndex < descriptor.imageViews.count(); imageViewIndex += 1) {
			const ref<Trident::ImageView> imageView = descriptor.imageViews[imageViewIndex];

			if (imageView->getImage()->getSampleCount() > 1) {
				resolve = true;

				break;
			}
		}

		uint32 boundAttachmentIndex = 0;

		for (uint32 imageViewIndex = 0; imageViewIndex < descriptor.imageViews.count(); imageViewIndex += 1) {
			const ref<Trident::ImageView> imageView = descriptor.imageViews[imageViewIndex];
			const ref<GL::ImageView> glImageView = std::static_pointer_cast<GL::ImageView>(imageView);
			const GL::Image* glImage = static_cast<const GL::Image*>(imageView->getImage());

			if (resolve && imageView->getImage()->getSampleCount() == 1) {
				continue;
			}

			GLenum attachment;

			const ImageFormat internalFormat = fromInternalFormat(glImage->internalFormat);

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
				case GL_TEXTURE_2D_MULTISAMPLE:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
				case GL_TEXTURE_RECTANGLE:
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glNamedFramebufferTexture(
							framebuffer->framebuffer,
							attachment,
							glImageView->texture,
							imageView->getMipLevelOffset()
						);
					#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
						glFramebufferTexture2D(
							GL_FRAMEBUFFER,
							attachment,
							glImage->target,
							glImageView->texture,
							imageView->getMipLevelOffset()
						);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif

					break;
				default:
					ADAPTER_ASSERT(false, "Unhandled image target ([]).", glImage->target);

					break;
			}

			boundAttachmentIndex += 1;
		}

		// Verify the status of the framebuffer.
		#if defined(BUILD_MODE_DEBUG)
			GLenum status = 0;

			#if OPENGL_VERSION >= OPENGL_VERSION_4_5
				status = glCheckNamedFramebufferStatus(framebuffer->framebuffer, GL_FRAMEBUFFER);
			#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
				status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			#else
				ENGINE_ERROR_UNSUPPORTED();
			#endif

			ADAPTER_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete.");
		#endif

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				glObjectLabel(GL_FRAMEBUFFER, framebuffer->framebuffer, static_cast<GLsizei>(descriptor.name.count()), &descriptor.name[0]);
			}
		#endif

		return framebuffer;
	}

	void GL::destroyFramebuffer(const Trident::Device* device, const Trident::Framebuffer* framebuffer) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Framebuffer* _framebuffer = static_cast<const GL::Framebuffer*>(framebuffer);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_0
			glDeleteFramebuffers(1, &_framebuffer->framebuffer);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}
}