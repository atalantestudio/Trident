#include "pch.hpp"

#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"

#include "Backend/OpenGL/Format.hpp"
#include "Backend/OpenGL/ImageType.hpp"
#include "Backend/OpenGL/ImageUsage.hpp"
#include "Backend/OpenGL/InternalFormat.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/API/Rendering/SampleCount.hpp"
#endif

namespace ProjectA {
	OpenGLImage::OpenGLImage(const ImageDescriptor& descriptor) :
		target(getGlImageType(descriptor.type, descriptor.sampleCount, descriptor.layerCount)),
		usage(descriptor.usage),
		internalFormat(getGlInternalFormat(descriptor.format)),
		format(getGlFormat(descriptor.format)),
		width(descriptor.width),
		height(descriptor.height)
	{
		PROFILE_SCOPE(FUNC_SIG);

		DEBUG_ASSERT(checkSampleCountValidity(descriptor.sampleCount));

		glCreateTextures(target, 1, &object);

		switch (target) {
			case GL_PROXY_TEXTURE_1D:
			case GL_TEXTURE_1D:
				// TODO: Test this path.
				DEBUG_ASSERT(false);

				break;
			case GL_PROXY_TEXTURE_1D_ARRAY:
			case GL_PROXY_TEXTURE_2D:
			case GL_PROXY_TEXTURE_CUBE_MAP:
			case GL_PROXY_TEXTURE_RECTANGLE:
			case GL_TEXTURE_1D_ARRAY:
			case GL_TEXTURE_2D:
			case GL_TEXTURE_CUBE_MAP:
			case GL_TEXTURE_RECTANGLE:
				if (descriptor.sampleCount > 1) {
					glTextureStorage2DMultisample(object, descriptor.sampleCount, internalFormat, descriptor.width, descriptor.height, GL_TRUE);
				}
				else {
					glTextureStorage2D(object, descriptor.mipLevelCount, internalFormat, descriptor.width, descriptor.height);
				}

				break;
			case GL_PROXY_TEXTURE_2D_ARRAY:
			case GL_PROXY_TEXTURE_3D:
			case GL_PROXY_TEXTURE_CUBE_MAP_ARRAY:
			case GL_TEXTURE_2D_ARRAY:
			case GL_TEXTURE_3D:
			case GL_TEXTURE_CUBE_MAP_ARRAY:
				ENGINE_ASSERT(descriptor.depth == 1, "The OpenGL adapter only supports texture arrays of depth 1.");

				if (descriptor.sampleCount > 1) {
					glTextureStorage3DMultisample(object, descriptor.sampleCount, internalFormat, width, height, descriptor.layerCount, GL_TRUE);
				}
				else {
					// glTextureStorage3D(object, 1, GL_SRGB8_ALPHA8, 512, 512, 4);
					// glTextureStorage3D(object, descriptor.mipLevelCount, internalFormat, descriptor.width, descriptor.height, descriptor.layerCount);
					glTextureStorage3D(object, 4, internalFormat, descriptor.width, descriptor.height, 1);
				}

				break;
		}

		// TODO: Check GL_TEXTURE_IMMUTABLE_FORMAT after creation.
		// glGetTextureParameteriv + GL_TEXTURE_IMMUTABLE_FORMAT
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
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		return createRef<OpenGLImageView>(*this, descriptor);
	}
}