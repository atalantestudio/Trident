#include "pch.hpp"

#include <stb_image.h>

#include "Platform/OpenGL/OpenGLTexture2D.hpp"

namespace ProjectA {
	OpenGLTexture2D::OpenGLTexture2D(GLsizei width, GLsizei height) :
		internalFormat(GL_RGBA8),
		format(GL_RGBA),
		width(width),
		height(height),
		debugPath("")
	{
		PROFILE_SCOPE(FUNC_SIG);

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
		PROFILE_SCOPE(FUNC_SIG);

		stbi_set_flip_vertically_on_load(1);

		int32 x;
		int32 y;
		int32 channelCount;
		stbi_uc* data;

		{
			PROFILE_SCOPE("OpenGLTexture2D/stbi_load");

			data = stbi_load(path.c_str(), &x, &y, &channelCount, 0);
		}

		ENGINE_ASSERT(data, "Could not load texture");

		width = x;
		height = y;

		if (channelCount == 3) {
			internalFormat = GL_RGB8;
			format = GL_RGB;
		}
		else if (channelCount == 4) {
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
		}

		ENGINE_ASSERT((channelCount == 3 || channelCount == 4), "Texture format not supported");

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
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteTextures(1, &texture);
	}

	uint32 OpenGLTexture2D::getId() const {
		return texture;
	}

	uint32 OpenGLTexture2D::getWidth() const {
		return static_cast<uint32>(width);
	}

	uint32 OpenGLTexture2D::getHeight() const {
		return static_cast<uint32>(height);
	}

	void OpenGLTexture2D::setData(const void* data, uint32 size) const {
		PROFILE_SCOPE(FUNC_SIG);

		// TODO: Store channel count?
		const uint32 channelCount = format == GL_RGBA ? 4 : 3;

		const bool ok = size == (width * height * channelCount);

		ENGINE_ASSERT(ok, "Data must be entire texture");

		glTextureSubImage2D(texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::bind(uint32 slot) const {
		PROFILE_SCOPE(FUNC_SIG);

		glBindTextureUnit(slot, texture);
	}

	void OpenGLTexture2D::unbind() const {
		PROFILE_SCOPE(FUNC_SIG);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	bool OpenGLTexture2D::operator ==(const Texture& other) const {
		return texture == ((OpenGLTexture2D&) other).texture;
	}
}