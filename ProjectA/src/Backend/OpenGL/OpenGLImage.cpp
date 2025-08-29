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
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateTextures(target, 1, &object);
		#elif OPENGL_VERSION >= OPENGL_VERSION_2_0
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
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glTextureStorage2DMultisample(object, sampleCount, internalFormat, extent.width, extent.height, GL_TRUE);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
						glTexStorage2DMultisample(target, sampleCount, internalFormat, extent.width, extent.height, GL_TRUE);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}
				else {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glTextureStorage2D(object, mipLevelCount, internalFormat, extent.width, extent.height);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_2
						glTexStorage2D(target, mipLevelCount, internalFormat, extent.width, extent.height);
					#else
						ENGINE_ERROR_UNSUPPORTED();
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
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glTextureStorage3DMultisample(object, sampleCount, internalFormat, extent.width, extent.height, extent.depth * layerCount, GL_TRUE);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
						glTexStorage3DMultisample(target, sampleCount, internalFormat, extent.width, extent.height, extent.depth * layerCount, GL_TRUE);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}
				else {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glTextureStorage3D(object, mipLevelCount, internalFormat, extent.width, extent.height, extent.depth * layerCount);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_2
						glTexStorage3D(target, mipLevelCount, internalFormat, extent.width, extent.height, extent.depth * layerCount);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}

				break;
			default:
				ENGINE_ERROR("Unhandled image target ([]).", target);
				DEBUG_ASSERT(false);

				break;
		}

		// Verify that the texture has an immutable format.
		#ifdef BUILD_MODE_DEBUG
			GLint immutableFormat = GL_FALSE;

			#if OPENGL_VERSION >= OPENGL_VERSION_4_5
				glGetTextureParameteriv(object, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
			#elif OPENGL_VERSION >= OPENGL_VERSION_2_0
				glGetTexParameteriv(target, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
			#endif

			ENGINE_ASSERT(immutableFormat != GL_FALSE, "Only immutable image formats are not supported.");
		#endif
	}

	OpenGLImage::~OpenGLImage() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteTextures(1, &object);
	}

	ref<ImageView> OpenGLImage::createViewInternal(const Image* image, const ImageViewDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor, image);

		return createRef<OpenGLImageView>(image, descriptor);
	}
}