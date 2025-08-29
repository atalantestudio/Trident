#include "pch.hpp"

#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"

#include "Backend/OpenGL/ImageType.hpp"

namespace ProjectA {
	OpenGLImageView::OpenGLImageView(const Image* image, const ImageViewDescriptor& descriptor) :
		ImageView(image, descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const OpenGLImage& glImage = *static_cast<const OpenGLImage*>(image);

		// NOTE: `glCreateTextures` cannot be used here because the texture view must not be bound or initialized.
		glGenTextures(1, &object);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3
			glTextureView(object, glImage.target, glImage.object, glImage.internalFormat, 0, image->getMipLevelCount(), 0, image->getLayerCount());
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	OpenGLImageView::~OpenGLImageView() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteTextures(1, &object);
	}
}