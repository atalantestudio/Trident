#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/ImageFormat.hpp"

namespace ProjectA {
	static GLenum getGlImageFormat(ImageFormat imageFormat) {
		static constexpr GLenum glImageFormats[] = {
			GL_R8,
			GL_SRGB8,
			GL_RGBA8,
		};

		return glImageFormats[static_cast<uint8>(imageFormat)];
	}
}