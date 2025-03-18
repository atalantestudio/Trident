#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/TextureType.hpp"

namespace ProjectA {
	static GLenum getGlTextureType(TextureType textureType) {
		static constexpr GLenum glDescriptorTypes[] = {
			GL_TEXTURE_2D,
		};

		return glDescriptorTypes[static_cast<uint8>(textureType)];
	}
}