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
		Image(descriptor),
		target(getGlImageType(descriptor.type, sampleCount, layerCount)),
		usage(descriptor.usage),
		internalFormat(getGlInternalFormat(descriptor.format)),
		format(getGlFormat(descriptor.format))
	{
		PROFILE_SCOPE(FUNC_SIG);

		DEBUG_ASSERT(checkSampleCountValidity(sampleCount));

		glCreateTextures(target, 1, &object);

		switch (target) {
			case GL_PROXY_TEXTURE_1D:
			case GL_TEXTURE_1D:
				// TODO: Test this path.
				DEBUG_ASSERT(false);

				break;
			case GL_PROXY_TEXTURE_1D_ARRAY:
			case GL_PROXY_TEXTURE_2D:
			case GL_PROXY_TEXTURE_2D_MULTISAMPLE:
			case GL_PROXY_TEXTURE_CUBE_MAP:
			case GL_PROXY_TEXTURE_RECTANGLE:
			case GL_TEXTURE_1D_ARRAY:
			case GL_TEXTURE_2D:
			case GL_TEXTURE_2D_MULTISAMPLE:
			case GL_TEXTURE_CUBE_MAP:
			case GL_TEXTURE_RECTANGLE:
				if (sampleCount > 1) {
					glTextureStorage2DMultisample(object, sampleCount, internalFormat, extent.width, extent.height, GL_TRUE);
				}
				else {
					glTextureStorage2D(object, mipLevelCount, internalFormat, extent.width, extent.height);
				}

				break;
			case GL_PROXY_TEXTURE_2D_ARRAY:
			case GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY:
			case GL_PROXY_TEXTURE_3D:
			case GL_PROXY_TEXTURE_CUBE_MAP_ARRAY:
			case GL_TEXTURE_2D_ARRAY:
			case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			case GL_TEXTURE_3D:
			case GL_TEXTURE_CUBE_MAP_ARRAY:
				if (sampleCount > 1) {
					glTextureStorage3DMultisample(object, sampleCount, internalFormat, extent.width, extent.height, extent.depth * layerCount, GL_TRUE);
				}
				else {
					glTextureStorage3D(object, mipLevelCount, internalFormat, extent.width, extent.height, extent.depth * layerCount);
				}

				break;
			default:
				ENGINE_ERROR("Unhandled image target ([]).", target);
				DEBUG_ASSERT(false);

				break;
		}

		glBindTexture(target, object);

		GLint immutableFormat;

		glGetTexParameteriv(target, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);

		ENGINE_ASSERT(immutableFormat != GL_FALSE, "Only immutable image formats are not supported.");

		glBindTexture(target, 0);
	}

	OpenGLImage::~OpenGLImage() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteTextures(1, &object);
	}

	ref<ImageView> OpenGLImage::createView(const ImageViewDescriptor& descriptor) const {
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		return createRef<OpenGLImageView>(*this, descriptor);
	}
}