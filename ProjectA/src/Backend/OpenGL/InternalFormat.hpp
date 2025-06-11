#pragma once

namespace ProjectA {
	static GLenum getGlInternalFormat(ImageFormat imageFormat) {
		static constexpr GLenum glInternalFormats[] = {
			GL_R8,
			GL_BGRA8_EXT,
			GL_SRGB8_ALPHA8,
			GL_RGBA8,
			GL_DEPTH_COMPONENT32F,
			GL_DEPTH32F_STENCIL8,
			GL_DEPTH24_STENCIL8,
		};

		return glInternalFormats[static_cast<uint8>(imageFormat)];
	}
}