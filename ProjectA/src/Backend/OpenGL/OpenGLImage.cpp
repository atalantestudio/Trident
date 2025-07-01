#include "pch.hpp"

#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"

#include "Backend/OpenGL/ImageFormat.hpp"
#include "Backend/OpenGL/ImageType.hpp"
#include "Backend/OpenGL/ImageUsage.hpp"

namespace ProjectA {
	OpenGLImage::OpenGLImage(const ImageDescriptor& descriptor) :
		Image(descriptor),
		target(getGlImageType(descriptor.type, sampleCount, layerCount)),
		usage(descriptor.usage),
		internalFormat(getGlInternalFormat(descriptor.format)),
		format(getGlFormat(descriptor.format))
	{
		PROFILE_SCOPE(FUNC_SIG);

		#ifdef OPENGL_USE_DSA
			glCreateTextures(target, 1, &object);
		#else
			glGenTextures(1, &object);
			glBindTexture(target, object);
		#endif

		switch (target) {
			case GL_PROXY_TEXTURE_1D:
			case GL_TEXTURE_1D:
				// TODO: Implement 1D images.
				ENGINE_ASSERT(false, "1D images are not supported.");

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
				// TODO: Decouple multisample from non-multisample images.
				if (sampleCount > 1) {
					#ifdef OPENGL_USE_DSA
						glTextureStorage2DMultisample(object, sampleCount, internalFormat, extent.width, extent.height, GL_TRUE);
					#else
						glTexStorage2DMultisample(target, sampleCount, internalFormat, extent.width, extent.height, GL_TRUE);
					#endif
				}
				else {
					#ifdef OPENGL_USE_DSA
						glTextureStorage2D(object, mipLevelCount, internalFormat, extent.width, extent.height);
					#else
						glTexStorage2D(target, mipLevelCount, internalFormat, extent.width, extent.height);
					#endif
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
				// TODO: Decouple multisample from non-multisample images.
				if (sampleCount > 1) {
					#ifdef OPENGL_USE_DSA
						glTextureStorage3DMultisample(object, sampleCount, internalFormat, extent.width, extent.height, extent.depth * layerCount, GL_TRUE);
					#else
						glTexStorage3DMultisample(target, sampleCount, internalFormat, extent.width, extent.height, extent.depth * layerCount, GL_TRUE);
					#endif
				}
				else {
					#ifdef OPENGL_USE_DSA
						glTextureStorage3D(object, mipLevelCount, internalFormat, extent.width, extent.height, extent.depth * layerCount);
					#else
						glTexStorage3D(target, mipLevelCount, internalFormat, extent.width, extent.height, extent.depth * layerCount);
					#endif
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

	ref<ImageView> OpenGLImage::createViewInternal(const Image* image, const ImageViewDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createRef<OpenGLImageView>(image, descriptor);
	}
}