// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/GL/ImageType.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Image::Image(const Trident::Device* device, const ImageDescriptor& descriptor) :
		Trident::Image(device, descriptor)
	{}

	ref<Trident::Image> GL::createImage(const Trident::Device* device, const ImageDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Image> image = allocateRef<GL::Image>(device, descriptor);
		image->target = getImageType(descriptor.type, image->sampleCount, image->layerCount);
		image->internalFormat = getInternalFormat(descriptor.format);
		image->format = GL::getFormat(descriptor.format);

		glGenTextures(1, &image->texture);
		glBindTexture(image->target, image->texture);

		switch (image->target) {
			case GL_PROXY_TEXTURE_1D:
			case GL_TEXTURE_1D: {
				// TODO(graphics/gl/image): Add support for 1D images.
				ADAPTER_ASSERT(false, "1D images are not supported.");

				break;
			}
			case GL_PROXY_TEXTURE_1D_ARRAY:
			case GL_PROXY_TEXTURE_2D:
			case GL_PROXY_TEXTURE_2D_MULTISAMPLE:
			case GL_PROXY_TEXTURE_CUBE_MAP:
			case GL_PROXY_TEXTURE_RECTANGLE:
			case GL_TEXTURE_1D_ARRAY:
			case GL_TEXTURE_2D:
			case GL_TEXTURE_2D_MULTISAMPLE:
			case GL_TEXTURE_CUBE_MAP:
			case GL_TEXTURE_RECTANGLE: {
				// TODO(graphics/gl/image): Decouple multisampled from non-multisampled images.
				if (image->sampleCount > 1) {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_3
						glTexStorage2DMultisample(image->target, image->sampleCount, image->internalFormat, image->extent.x, image->extent.y, GL_TRUE);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}
				else {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_2
						glTexStorage2D(image->target, image->mipLevelCount, image->internalFormat, image->extent.x, image->extent.y);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}

				break;
			}
			case GL_PROXY_TEXTURE_2D_ARRAY:
			case GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY:
			case GL_PROXY_TEXTURE_3D:
			case GL_PROXY_TEXTURE_CUBE_MAP_ARRAY:
			case GL_TEXTURE_2D_ARRAY:
			case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			case GL_TEXTURE_3D:
			case GL_TEXTURE_CUBE_MAP_ARRAY: {
				// TODO(graphics/gl/image): Decouple multisampled from non-multisampled images.
				if (image->sampleCount > 1) {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_3
						glTexStorage3DMultisample(image->target, image->sampleCount, image->internalFormat, image->extent.x, image->extent.y, image->extent.z * image->layerCount, GL_TRUE);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}
				else {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_2
						glTexStorage3D(image->target, image->mipLevelCount, image->internalFormat, image->extent.x, image->extent.y, image->extent.z * image->layerCount);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}

				break;
			}
			default: {
				ADAPTER_ASSERT(false, "Unhandled image target ([]).", image->target);

				break;
			}
		}

		#if defined(BUILD_MODE_DEBUG)
			GLint immutableFormat = GL_FALSE;

			glGetTexParameteriv(image->target, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);

			ADAPTER_ASSERT(immutableFormat != GL_FALSE, "Only immutable image formats are not supported.");
		#endif

		if (image->sampleCount == 1) {
			glTexParameteri(image->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(image->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(image->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(image->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3 && defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				glObjectLabel(GL_TEXTURE, image->texture, static_cast<GLsizei>(descriptor.name.count()), &descriptor.name[0]);
			}
		#endif

		return image;
	}

	void GL::destroyImage(const Trident::Device* device, const Trident::Image* image) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Image* _image = static_cast<const GL::Image*>(image);

		glDeleteTextures(1, &_image->texture);
	}
}