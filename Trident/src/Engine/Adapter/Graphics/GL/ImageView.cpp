// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageType.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"

namespace Trident {
	GL::ImageView::ImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor) :
		Trident::ImageView(device, image, descriptor)
	{}

	ref<Trident::ImageView> GL::createImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::ImageView> imageView = allocateRef<GL::ImageView>(device, image, descriptor);

		const GL::Image* _image = static_cast<const GL::Image*>(image);

		glGenTextures(1, &imageView->texture);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3
			glTextureView(imageView->texture, _image->target, _image->texture, _image->internalFormat, 0, _image->getMipLevelCount(), 0, _image->getLayerCount());
		#else
			// TODO(graphics/gl/image-view): Revert to a simple proxy of the parent image.
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3 && defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				glObjectLabel(GL_TEXTURE, imageView->texture, static_cast<GLsizei>(descriptor.name.count()), &descriptor.name[0]);
			}
		#endif

		return imageView;
	}

	void GL::destroyImageView(const Trident::Device* device, const Trident::ImageView* imageView) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::ImageView* _imageView = static_cast<const GL::ImageView*>(imageView);

		glDeleteTextures(1, &_imageView->texture);
	}
}