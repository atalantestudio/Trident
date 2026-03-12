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

		ref<GL::ImageView> imageView = createRef<GL::ImageView>(device, image, descriptor);

		const GL::Image* _image = static_cast<const GL::Image*>(image);

		// NOTE: `glCreateTextures` cannot be used here because the texture view must not be bound or initialized.
		glGenTextures(1, &imageView->texture);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3
			glTextureView(imageView->texture, _image->target, _image->texture, _image->internalFormat, 0, _image->getMipLevelCount(), 0, _image->getLayerCount());
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		return imageView;
	}

	void GL::destroyImageView(const Trident::Device* device, const Trident::ImageView* imageView) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::ImageView* _imageView = static_cast<const GL::ImageView*>(imageView);

		glDeleteTextures(1, &_imageView->texture);
	}
}