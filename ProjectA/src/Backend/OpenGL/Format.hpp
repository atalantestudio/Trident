#pragma once

namespace ProjectA {
	static GLenum getGlFormat(ImageFormat imageFormat) {
		static constexpr GLenum glFormats[] = {
			GL_RED,
			GL_BGRA,
			GL_RGBA,
			GL_RGBA,
			GL_DEPTH_COMPONENT,
			GL_DEPTH_STENCIL,
			GL_DEPTH_STENCIL,
		};

		return glFormats[static_cast<uint8>(imageFormat)];
	}
}