#include "pch.hpp"

#include "Backend/OpenGL/Format.hpp"
#include "Backend/OpenGL/ImageType.hpp"
#include "Backend/OpenGL/InternalFormat.hpp"
#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"

namespace ProjectA {
	OpenGLImage::OpenGLImage(const ImageDescriptor& descriptor) :
		target(getGlImageType(descriptor.type)),
		internalFormat(getGlInternalFormat(descriptor.format)),
		format(getGlFormat(descriptor.format)),
		width(descriptor.width),
		height(descriptor.height)
	{
		PROFILE_SCOPE(FUNC_SIG);

		glCreateTextures(target, 1, &object);
		glTextureStorage2D(object, descriptor.mipLevelCount, internalFormat, descriptor.width, descriptor.height);
	}

	OpenGLImage::~OpenGLImage() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteTextures(1, &object);
	}

	uint32 OpenGLImage::getWidth() const {
		return width;
	}

	uint32 OpenGLImage::getHeight() const {
		return height;
	}

	ref<ImageView> OpenGLImage::createView(const ImageViewDescriptor& descriptor) const {
		return createRef<OpenGLImageView>(*this, descriptor);
	}
}