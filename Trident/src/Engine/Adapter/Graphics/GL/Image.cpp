// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/GL/ImageType.hpp"
#include "Engine/Adapter/Graphics/GL/ImageUsage.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Image::Image(const Trident::Device* device, const ImageDescriptor& descriptor) :
		Trident::Image(device, descriptor)
	{}

	ref<Trident::Image> GL::createImage(const Trident::Device* device, const ImageDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Image> image = createRef<GL::Image>(device, descriptor);
		image->target = getImageType(descriptor.type, image->sampleCount, image->layerCount);
		image->usage = descriptor.usage;
		image->internalFormat = getInternalFormat(descriptor.format);
		image->format = GL::getFormat(descriptor.format);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateTextures(image->target, 1, &image->texture);
		#else
			glGenTextures(1, &image->texture);
			glBindTexture(image->target, image->texture);
		#endif

		switch (image->target) {
			case GL_PROXY_TEXTURE_1D:
			case GL_TEXTURE_1D:
				// TODO: Implement 1D images.
				ADAPTER_ASSERT(false, "1D images are not supported.");

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
				if (image->sampleCount > 1) {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glTextureStorage2DMultisample(image->texture, image->sampleCount, image->internalFormat, image->extent.width, image->extent.height, GL_TRUE);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
						glTexStorage2DMultisample(image->target, image->sampleCount, image->internalFormat, image->extent.width, image->extent.height, GL_TRUE);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}
				else {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glTextureStorage2D(image->texture, image->mipLevelCount, image->internalFormat, image->extent.width, image->extent.height);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_2
						glTexStorage2D(image->target, image->mipLevelCount, image->internalFormat, image->extent.width, image->extent.height);
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
				if (image->sampleCount > 1) {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glTextureStorage3DMultisample(image->texture, image->sampleCount, image->internalFormat, image->extent.width, image->extent.height, image->extent.depth * image->layerCount, GL_TRUE);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
						glTexStorage3DMultisample(image->target, image->sampleCount, image->internalFormat, image->extent.width, image->extent.height, image->extent.depth * image->layerCount, GL_TRUE);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}
				else {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glTextureStorage3D(image->texture, image->mipLevelCount, image->internalFormat, image->extent.width, image->extent.height, image->extent.depth * image->layerCount);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_2
						glTexStorage3D(image->target, image->mipLevelCount, image->internalFormat, image->extent.width, image->extent.height, image->extent.depth * image->layerCount);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}

				break;
			default:
				ADAPTER_ASSERT(false, "Unhandled image target ([]).", image->target);

				break;
		}

		// Verify that the texture has an immutable format.
		#if defined(BUILD_MODE_DEBUG)
			GLint immutableFormat = GL_FALSE;

			#if OPENGL_VERSION >= OPENGL_VERSION_4_5
				glGetTextureParameteriv(image->texture, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
			#else
				glGetTexParameteriv(image->target, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
			#endif

			ADAPTER_ASSERT(immutableFormat != GL_FALSE, "Only immutable image formats are not supported.");
		#endif

		// Set default texture parameters.
		if (image->sampleCount == 1) {
			#if OPENGL_VERSION >= OPENGL_VERSION_4_5
				glTextureParameteri(image->texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(image->texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTextureParameteri(image->texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(image->texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			#else
				glTexParameteri(image->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(image->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(image->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(image->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			#endif
		}

		#if OPENGL_VERSION < OPENGL_VERSION_4_5 && defined(OPENGL_UNBIND_AFTER_USE)
			glBindTexture(image->target, 0);
		#endif

		return image;
	}

	ref<Trident::Image> GL::createImage(const Trident::Device* device, VkImage image, VkFormat format, Extent2D extent, uint32 index) {
		ADAPTER_ASSERT(false, "Swapchain images are implicitly handled by the adapter.");

		return nullptr;
	}

	void GL::destroyImage(const Trident::Device* device, const Trident::Image* image) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Image* _image = static_cast<const GL::Image*>(image);

		glDeleteTextures(1, &_image->texture);
	}
}