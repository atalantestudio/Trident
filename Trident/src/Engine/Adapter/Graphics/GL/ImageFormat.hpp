// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/ImageFormat.hpp"

namespace Trident::GL {
	// See https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexImage2D.xhtml.
	inline GLenum getFormat(ImageFormat imageFormat) {
		static constexpr GLenum FORMATS[] = {
			GL_RED,
			GL_RGB,
			GL_BGRA,
			GL_RGBA,
			GL_RGBA,
			GL_RGBA,
			GL_RED_INTEGER,
			GL_RG_INTEGER,
			GL_DEPTH_COMPONENT,
			GL_DEPTH_STENCIL,
			GL_DEPTH_STENCIL,
		};

		ADAPTER_ASSERT(imageFormat < ImageFormat::count, "Invalid image format.");

		return FORMATS[static_cast<uint32>(imageFormat)];
	}

	// See https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexImage2D.xhtml.
	inline GLenum getInternalFormat(ImageFormat imageFormat) {
		static constexpr GLenum INTERNAL_FORMATS[] = {
			GL_R8,
			GL_RGB8,
			GL_BGRA8_EXT,
			GL_SRGB8_ALPHA8,
			GL_RGBA8,
			GL_RGBA32F,
			GL_R32UI,
			GL_RG32UI,
			GL_DEPTH_COMPONENT32F,
			GL_DEPTH32F_STENCIL8,
			GL_DEPTH24_STENCIL8,
		};

		ADAPTER_ASSERT(imageFormat < ImageFormat::count, "Invalid image format.");

		return INTERNAL_FORMATS[static_cast<uint32>(imageFormat)];
	}

	inline GLenum getType(ImageFormat imageFormat) {
		static constexpr GLenum TYPES[] = {
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_FLOAT,
			GL_UNSIGNED_INT,
			GL_UNSIGNED_INT,
		};

		ADAPTER_ASSERT(imageFormat < ImageFormat::D32_SFLOAT, "Invalid image format.");

		return TYPES[static_cast<uint32>(imageFormat)];
	}

	inline ImageFormat fromInternalFormat(GLenum internalFormat) {
		switch (internalFormat) {
			case GL_R8: return ImageFormat::R8_UNORM;
			case GL_RGB8: return ImageFormat::R8G8B8_UNORM;
			case GL_BGRA8_EXT: return ImageFormat::B8G8R8A8_UNORM;
			case GL_SRGB8_ALPHA8: return ImageFormat::R8G8B8A8_SRGB;
			case GL_RGBA8: return ImageFormat::R8G8B8A8_UNORM;
			case GL_RGBA32F: return ImageFormat::R32G32B32A32_SFLOAT;
			case GL_R32UI: return ImageFormat::R32_UINT;
			case GL_RG32UI: return ImageFormat::R32G32_UINT;
			case GL_DEPTH_COMPONENT32F: return ImageFormat::D32_SFLOAT;
			case GL_DEPTH32F_STENCIL8: return ImageFormat::D32_SFLOAT_S8_UINT;
			case GL_DEPTH24_STENCIL8: return ImageFormat::D24_UNORM_S8_UINT;
			default: {
				ADAPTER_ASSERT(false, "Invalid internal format.");

				return static_cast<ImageFormat>(0);
			}
		}
	}

	inline bool isDepthFormat(GLenum internalFormat) {
		return (
			internalFormat == GL_DEPTH_COMPONENT32F ||
			internalFormat == GL_DEPTH32F_STENCIL8 ||
			internalFormat == GL_DEPTH24_STENCIL8
		);
	}

	inline bool isStencilFormat(GLenum internalFormat) {
		return (
			internalFormat == GL_DEPTH32F_STENCIL8 ||
			internalFormat == GL_DEPTH24_STENCIL8
		);
	}
}