#pragma once

#include "Engine/Renderer/ImageFormat.hpp"

namespace ProjectA {
	static GLenum getGlFormat(ImageFormat imageFormat) {
		static constexpr GLenum glFormats[] = {
			GL_RED,
			GL_RGBA,
			GL_RGBA,
		};

		return glFormats[static_cast<uint8>(imageFormat)];
	}
}