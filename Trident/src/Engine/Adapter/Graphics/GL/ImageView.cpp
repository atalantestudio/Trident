// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageType.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"

namespace Trident {
	GL::ImageView::ImageView(const ImageViewDescriptor& descriptor, Trident::Image* image) :
		Trident::ImageView(descriptor, image)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Image* _image = static_cast<const GL::Image*>(image);

		// NOTE: `glCreateTextures` cannot be used here because the texture view must not be bound or initialized.
		glGenTextures(1, &texture);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3
			glTextureView(texture, _image->target, _image->texture, _image->internalFormat, 0, _image->getMipLevelCount(), 0, _image->getLayerCount());
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	GL::ImageView::~ImageView() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteTextures(1, &texture);
	}
}