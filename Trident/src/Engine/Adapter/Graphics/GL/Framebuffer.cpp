// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	// All images attached to a framebuffer must have the same number of samples.
	// Use this information to find if the framebuffer contains multisampled attachments or not.
	static bool needsResolving(view<ref<ImageView>> imageViews) {
		for (ref<ImageView> imageView : imageViews) {
			if (imageView->getImage()->getSampleCount() > 1) {
				return true;
			}
		}

		return false;
	}

	GL::Framebuffer::Framebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor) :
		Trident::Framebuffer(device, descriptor)
	{}

	ref<Trident::Framebuffer> GL::createFramebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Framebuffer> framebuffer = allocateRef<GL::Framebuffer>(device, descriptor);

		const bool resolve = needsResolving(descriptor.imageViews);

		GLuint* framebuffers[] = {&framebuffer->framebuffer, &framebuffer->resolveFramebuffer};

		glGenFramebuffers(1 + resolve, *framebuffers);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->framebuffer);

		if (resolve) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->resolveFramebuffer);
		}

		uint32 imageViewIndex = 0;
		uint32 boundAttachmentIndex = 0;
		uint32 resolveAttachmentIndex = 0;

		while (imageViewIndex < descriptor.imageViews.count()) {
			const ref<Trident::ImageView> imageView = descriptor.imageViews[imageViewIndex];
			const ref<GL::ImageView> glImageView = ref_cast<GL::ImageView>(imageView);
			const GL::Image* glImage = static_cast<const GL::Image*>(imageView->getImage());

			if (resolve && imageView->getImage()->getSampleCount() == 1) {
				glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + resolveAttachmentIndex, glImage->target, glImageView->texture, imageView->getMipLevelOffset());

				imageViewIndex += 1;
				resolveAttachmentIndex += 1;

				continue;
			}

			GLenum attachment;

			const ImageFormat internalFormat = fromInternalFormat(glImage->internalFormat);

			// TODO: Handle stencil-only internal formats with GL_STENCIL_ATTACHMENT.
			switch (internalFormat) {
				case ImageFormat::D32_SFLOAT: {
					attachment = GL_DEPTH_ATTACHMENT;

					break;
				}
				case ImageFormat::D32_SFLOAT_S8_UINT:
				case ImageFormat::D24_UNORM_S8_UINT: {
					attachment = GL_DEPTH_STENCIL_ATTACHMENT;

					break;
				}
				default: {
					attachment = GL_COLOR_ATTACHMENT0 + boundAttachmentIndex;

					break;
				}
			}

			// TODO(graphics/gl/texture): Move that switch out of the loop. The function to call should be known beforehand (maybe even before creating the image).
			switch (glImage->target) {
				case GL_TEXTURE_2D:
				case GL_TEXTURE_2D_MULTISAMPLE:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
				case GL_TEXTURE_RECTANGLE: {
					glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, glImage->target, glImageView->texture, imageView->getMipLevelOffset());

					break;
				}
				default: {
					ADAPTER_ASSERT(false, "Unhandled image target ([]).", glImage->target);

					break;
				}
			}

			boundAttachmentIndex += 1;
			imageViewIndex += 1;
		}

		#if defined(BUILD_MODE_DEBUG)
			GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

			ADAPTER_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete.");

			if (resolve) {
				status = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER);

				ADAPTER_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Resolve framebuffer is incomplete.");
			}
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3 && defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				glObjectLabel(GL_FRAMEBUFFER, framebuffer->framebuffer, static_cast<GLsizei>(descriptor.name.count()), &descriptor.name[0]);

				if (resolve) {
					sequence<char8> name(descriptor.name.count() + 11);

					std::copy(descriptor.name.begin(), descriptor.name.end(), &name[0]);
					std::copy_n(" (resolve)", 10, &name[descriptor.name.count()]);

					glObjectLabel(GL_FRAMEBUFFER, framebuffer->resolveFramebuffer, static_cast<GLsizei>(name.count()), &name[0]);
				}
			}
		#endif

		return framebuffer;
	}

	void GL::destroyFramebuffer(const Trident::Device* device, const Trident::Framebuffer* framebuffer) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Framebuffer* _framebuffer = static_cast<const GL::Framebuffer*>(framebuffer);
		const GLuint framebuffers[] = {_framebuffer->framebuffer, _framebuffer->resolveFramebuffer};
		const bool resolve = _framebuffer->resolveFramebuffer > 0;

		glDeleteFramebuffers(1 + resolve, framebuffers);
	}
}