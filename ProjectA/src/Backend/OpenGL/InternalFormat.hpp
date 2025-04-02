#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/ImageFormat.hpp"

namespace ProjectA {
	static GLenum getGlInternalFormat(ImageFormat imageFormat) {
		static constexpr GLenum glInternalFormats[] = {
			GL_R8,
			GL_SRGB8,
			GL_RGBA8,
		};

		return glInternalFormats[static_cast<uint8>(imageFormat)];
	}
}