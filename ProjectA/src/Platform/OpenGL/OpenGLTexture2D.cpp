#include "pch.hpp"
#include <glad/glad.h>
#include "stb_image.h"
#include "OpenGLTexture2D.hpp"

namespace ProjectA {
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : debugPath(path) {
		int x;
		int y;
		int channelCount;

		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = stbi_load(path.c_str(), &x, &y, &channelCount, 0);

		CORE_ASSERT(data, "Could not load texture");

		width = x;
		height = y;

		GLenum internalFormat = 0;
		GLenum format = 0;

		if (channelCount == 3) {
			internalFormat = GL_RGB8;
			format = GL_RGB;
		} else if (channelCount == 4) {
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
		}

		CORE_ASSERT(internalFormat & format, "Texture format not supported");

		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		glTextureStorage2D(texture, 1, internalFormat, width, height);
		glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureSubImage2D(texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D() {
		glDeleteTextures(1, &texture);
	}

	uint32_t OpenGLTexture2D::getWidth() const {
		return width;
	}

	uint32_t OpenGLTexture2D::getHeight() const {
		return height;
	}

	void OpenGLTexture2D::bind(uint32_t slot) const {
		glBindTextureUnit(slot, texture);
	}
}