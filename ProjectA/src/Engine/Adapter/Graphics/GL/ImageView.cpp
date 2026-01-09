// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageType.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"

namespace ProjectA {
	GL::ImageView::ImageView(const ImageViewDescriptor& descriptor, const ProjectA::Image* image) :
		ProjectA::ImageView(descriptor, image)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Image* glImage = static_cast<const GL::Image*>(image);

		// NOTE: `glCreateTextures` cannot be used here because the texture view must not be bound or initialized.
		glGenTextures(1, &texture);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3
			glTextureView(texture, glImage->target, glImage->texture, glImage->internalFormat, 0, glImage->getMipLevelCount(), 0, glImage->getLayerCount());
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	GL::ImageView::~ImageView() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteTextures(1, &texture);
	}
}