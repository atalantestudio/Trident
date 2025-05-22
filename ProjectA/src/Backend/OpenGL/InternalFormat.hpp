#pragma once

namespace ProjectA {
	static GLenum getGlInternalFormat(ImageFormat imageFormat) {
		static constexpr GLenum glInternalFormats[] = {
			GL_R8,
			GL_BGRA8_EXT,
			GL_SRGB8,
			GL_RGBA8,
			GL_DEPTH_COMPONENT,
			GL_DEPTH_COMPONENT,
			GL_DEPTH_COMPONENT,
		};

		return glInternalFormats[static_cast<uint8>(imageFormat)];
	}
}