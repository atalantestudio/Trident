#include "pch.hpp"

#include <stb_image.h>

#include "Backend/OpenGL/OpenGLTexture.hpp"

namespace ProjectA {
	OpenGLTexture::OpenGLTexture(GLsizei width, GLsizei height) :
		internalFormat(GL_RGBA8),
		format(GL_RGBA),
		width(width),
		height(height),
		debugPath("")
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[OpenGLTexture]");

		glCreateTextures(GL_TEXTURE_2D, 1, &object);

		glTextureStorage2D(object, 1, internalFormat, width, height);

		glTextureParameteri(object, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(object, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(object, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(object, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture::OpenGLTexture() :
		internalFormat(GL_RGBA8),
		format(GL_RGBA)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[OpenGLTexture]");

		stbi_set_flip_vertically_on_load(1);

		int32 x;
		int32 y;
		int32 channelCount;
		stbi_uc* data;

		{
			PROFILE_SCOPE("OpenGLTexture/stbi_load");

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

		glCreateTextures(GL_TEXTURE_2D, 1, &object);

		glTextureStorage2D(object, 1, internalFormat, width, height);

		glTextureParameteri(object, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(object, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(object, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(object, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(object, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture::~OpenGLTexture() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~OpenGLTexture]");

		glDeleteTextures(1, &object);
	}

	uint32 OpenGLTexture::getWidth() const {
		return static_cast<uint32>(width);
	}

	uint32 OpenGLTexture::getHeight() const {
		return static_cast<uint32>(height);
	}

	void OpenGLTexture::setData(const void* data, uint32 size) const {
		PROFILE_SCOPE(FUNC_SIG);

		// TODO: Store channel count?
		const uint32 channelCount = format == GL_RGBA ? 4 : 3;

		const bool ok = size == (width * height * channelCount);

		ENGINE_ASSERT(ok, "Data must be entire texture.");

		glTextureSubImage2D(object, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
	}

	/*void OpenGLTexture::bind(uint32 slot) const {
		PROFILE_SCOPE(FUNC_SIG);

		glBindTextureUnit(slot, object);
	}

	void OpenGLTexture::unbind() const {
		PROFILE_SCOPE(FUNC_SIG);

		glBindTexture(GL_TEXTURE_2D, 0);
	}*/
}