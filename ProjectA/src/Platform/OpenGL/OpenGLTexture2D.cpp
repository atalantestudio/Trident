#include "pch.hpp"

#include <glad/glad.h>
#include <stb_image.h>

#include "OpenGLTexture2D.hpp"

namespace ProjectA {
	OpenGLTexture2D::OpenGLTexture2D(std::size_t width, std::size_t height) :
		internalFormat(GL_RGBA8),
		format(GL_RGBA),
		width(width),
		height(height),
		debugPath("")
	{
		PROFILE_SCOPE(__FUNCSIG__);

		glCreateTextures(GL_TEXTURE_2D, 1, &texture);

		glTextureStorage2D(texture, 1, internalFormat, width, height);

		glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) :
		internalFormat(GL_RGBA8),
		format(GL_RGBA),
		debugPath(path)
	{
		PROFILE_SCOPE(__FUNCSIG__);

		stbi_set_flip_vertically_on_load(1);

		int x;
		int y;
		int channelCount;
		stbi_uc* data;

		{
			PROFILE_SCOPE("OpenGLTexture2D/stbi_load");

			data = stbi_load(path.c_str(), &x, &y, &channelCount, 0);
		}

		CORE_ASSERT(data, "Could not load texture");

		width = x;
		height = y;

		if (channelCount == 3) {
			internalFormat = GL_RGB8;
			format = GL_RGB;
		}

		CORE_ASSERT(channelCount == 3 || channelCount == 4, "Texture format not supported");

		glCreateTextures(GL_TEXTURE_2D, 1, &texture);

		glTextureStorage2D(texture, 1, internalFormat, width, height);

		glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D() {
		PROFILE_SCOPE(__FUNCSIG__);

		glDeleteTextures(1, &texture);
	}

	uint32_t OpenGLTexture2D::getWidth() const {
		return width;
	}

	uint32_t OpenGLTexture2D::getHeight() const {
		return height;
	}

	void OpenGLTexture2D::bind(uint32_t slot) const {
		PROFILE_SCOPE(__FUNCSIG__);

		glBindTextureUnit(slot, texture);
	}

	void OpenGLTexture2D::unbind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture2D::setData(const void* data) const {
		PROFILE_SCOPE(__FUNCSIG__);

		glTextureSubImage2D(texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
	}
}