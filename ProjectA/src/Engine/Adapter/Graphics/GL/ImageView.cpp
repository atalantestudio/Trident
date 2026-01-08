#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageType.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"

namespace ProjectA {
	OpenGLImageView::OpenGLImageView(const ImageViewDescriptor& descriptor, const Image* image) :
		ImageView(descriptor, image)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const OpenGLImage* glImage = static_cast<const OpenGLImage*>(image);

		// NOTE: `glCreateTextures` cannot be used here because the texture view must not be bound or initialized.
		glGenTextures(1, &object);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3
			glTextureView(object, glImage->target, glImage->object, glImage->internalFormat, 0, glImage->getMipLevelCount(), 0, glImage->getLayerCount());
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	OpenGLImageView::~OpenGLImageView() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteTextures(1, &object);
	}
}