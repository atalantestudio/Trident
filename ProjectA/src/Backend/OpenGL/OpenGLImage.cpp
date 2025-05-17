#include "pch.hpp"

#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"

#include "Backend/OpenGL/Format.hpp"
#include "Backend/OpenGL/ImageType.hpp"
#include "Backend/OpenGL/InternalFormat.hpp"

namespace ProjectA {
	OpenGLImage::OpenGLImage(const ImageDescriptor& descriptor) :
		target(getGlImageType(descriptor.type, descriptor.sampleCount)),
		internalFormat(getGlInternalFormat(descriptor.format)),
		format(getGlFormat(descriptor.format)),
		width(descriptor.width),
		height(descriptor.height)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_ASSERT(descriptor.sampleCount > 0, "Could not create image: The sample count must be non-zero.");

		glCreateTextures(target, 1, &object);

		if (descriptor.sampleCount == 1) {
			glTextureStorage2D(object, descriptor.mipLevelCount, internalFormat, descriptor.width, descriptor.height);
		}
		else {
			glTextureStorage2DMultisample(object, descriptor.sampleCount, internalFormat, descriptor.width, descriptor.height, GL_TRUE);
		}
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