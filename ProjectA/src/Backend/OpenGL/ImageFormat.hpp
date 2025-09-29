#pragma once

namespace ProjectA {
	static GLenum getGlFormat(ImageFormat imageFormat) {
		static constexpr GLenum glFormats[] = {
			GL_RED,
			GL_RGB,
			GL_BGRA,
			GL_RGBA,
			GL_RGBA,
			GL_RED_INTEGER,
			GL_RG_INTEGER,
			GL_DEPTH_COMPONENT,
			GL_DEPTH_STENCIL,
			GL_DEPTH_STENCIL,
		};

		return glFormats[static_cast<uint8>(imageFormat)];
	}

	static GLenum getGlInternalFormat(ImageFormat imageFormat) {
		static constexpr GLenum glInternalFormats[] = {
			GL_R8,
			GL_RGB8,
			GL_BGRA8_EXT,
			GL_SRGB8_ALPHA8,
			GL_RGBA8,
			GL_R32UI,
			GL_RG32UI,
			GL_DEPTH_COMPONENT32F,
			GL_DEPTH32F_STENCIL8,
			GL_DEPTH24_STENCIL8,
		};

		return glInternalFormats[static_cast<uint8>(imageFormat)];
	}

	static ImageFormat fromGlInternalFormat(GLenum internalFormat) {
		switch (internalFormat) {
			case GL_R8:
				return ImageFormat::R8_UNORM;
			case GL_RGB8:
				return ImageFormat::R8G8B8_UNORM;
			case GL_BGRA8_EXT:
				return ImageFormat::B8G8R8A8_UNORM;
			case GL_SRGB8_ALPHA8:
				return ImageFormat::R8G8B8A8_SRGB;
			case GL_RGBA8:
				return ImageFormat::R8G8B8A8_UNORM;
			case GL_R32UI:
				return ImageFormat::R32_UINT;
			case GL_RG32UI:
				return ImageFormat::R32G32_UINT;
			case GL_DEPTH_COMPONENT32F:
				return ImageFormat::D32_SFLOAT;
			case GL_DEPTH32F_STENCIL8:
				return ImageFormat::D32_SFLOAT_S8_UINT;
			case GL_DEPTH24_STENCIL8:
				return ImageFormat::D24_UNORM_S8_UINT;
			default:
				ENGINE_ASSERT(false, "Unhandled OpenGL internal format.");

				return static_cast<ImageFormat>(0);
		}
	}

	static bool isDepthFormat(GLenum internalFormat) {
		return (
			internalFormat == GL_DEPTH_COMPONENT32F ||
			internalFormat == GL_DEPTH32F_STENCIL8 ||
			internalFormat == GL_DEPTH24_STENCIL8
		);
	}

	static bool isStencilFormat(GLenum internalFormat) {
		return (
			internalFormat == GL_DEPTH32F_STENCIL8 ||
			internalFormat == GL_DEPTH24_STENCIL8
		);
	}
}