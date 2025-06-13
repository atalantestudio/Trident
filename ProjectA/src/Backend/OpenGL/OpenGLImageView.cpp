#include "pch.hpp"

#include "Backend/OpenGL/OpenGLImageView.hpp"

#include "Backend/OpenGL/ImageType.hpp"

namespace ProjectA {
	OpenGLImageView::OpenGLImageView(const OpenGLImage& image, const ImageViewDescriptor& descriptor) :
		image(image)
	{
		PROFILE_SCOPE(FUNC_SIG);

		// `glGenTextures` must be used here instead of `glCreateTextures` because the view must not be assigned a target.
		glGenTextures(1, &object);
		glTextureView(object, image.target, image.object, image.internalFormat, 0, image.getMipLevelCount(), 0, image.getLayerCount());
	}

	OpenGLImageView::~OpenGLImageView() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteTextures(1, &object);
	}
}