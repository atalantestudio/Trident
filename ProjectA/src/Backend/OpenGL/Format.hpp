#pragma once

namespace ProjectA {
	static GLenum getGlFormat(ImageFormat imageFormat) {
		static constexpr GLenum glFormats[] = {
			GL_RED,
			GL_RGBA,
			GL_RGBA,
			GL_DEPTH_COMPONENT32F,
			GL_DEPTH32F_STENCIL8,
			GL_DEPTH24_STENCIL8,
		};

		return glFormats[static_cast<uint8>(imageFormat)];
	}
}