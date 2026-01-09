// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/GL/ImageType.hpp"
#include "Engine/Adapter/Graphics/GL/ImageUsage.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"

namespace ProjectA {
	GL::Image::Image(const ImageDescriptor& descriptor) :
		ProjectA::Image(descriptor),
		target(getImageType(descriptor.type, sampleCount, layerCount)),
		usage(descriptor.usage),
		internalFormat(getInternalFormat(descriptor.format)),
		format(GL::getFormat(descriptor.format))
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateTextures(target, 1, &texture);
		#else
			glGenTextures(1, &texture);
			glBindTexture(target, texture);
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
						glTextureStorage2DMultisample(texture, sampleCount, internalFormat, extent.width, extent.height, GL_TRUE);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
						glTexStorage2DMultisample(target, sampleCount, internalFormat, extent.width, extent.height, GL_TRUE);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}
				else {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glTextureStorage2D(texture, mipLevelCount, internalFormat, extent.width, extent.height);
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
						glTextureStorage3DMultisample(texture, sampleCount, internalFormat, extent.width, extent.height, extent.depth * layerCount, GL_TRUE);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
						glTexStorage3DMultisample(target, sampleCount, internalFormat, extent.width, extent.height, extent.depth * layerCount, GL_TRUE);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}
				else {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glTextureStorage3D(texture, mipLevelCount, internalFormat, extent.width, extent.height, extent.depth * layerCount);
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
				glGetTextureParameteriv(texture, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
			#else
				glGetTexParameteriv(target, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
			#endif

			ENGINE_ASSERT(immutableFormat != GL_FALSE, "Only immutable image formats are not supported.");
		#endif

		// Set default texture parameters.
		if (sampleCount == 1) {
			#if OPENGL_VERSION >= OPENGL_VERSION_4_5
				glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			#else
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			#endif
		}

		#if OPENGL_VERSION < OPENGL_VERSION_4_5 && defined(OPENGL_UNBIND_AFTER_USE)
			glBindTexture(target, 0);
		#endif
	}

	GL::Image::~Image() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteTextures(1, &texture);
	}

	ref<ImageView> GL::Image::createViewInternal(const ImageViewDescriptor& descriptor, const ProjectA::Image* image) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor, image);

		return createRef<GL::ImageView>(descriptor, image);
	}
}