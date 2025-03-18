#include "pch.hpp"

#include "Backend/OpenGL/OpenGLTexture.hpp"
#include "Backend/OpenGL/TextureType.hpp"

namespace ProjectA {
	OpenGLTexture::OpenGLTexture(const TextureDescriptor& descriptor) :
		target(getGlTextureType(descriptor.type)),
		width(descriptor.width),
		height(descriptor.height)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[OpenGLTexture]");

		glCreateTextures(target, 1, &object);

		glTextureStorage2D(object, descriptor.mipLevelCount, GL_RGBA8, descriptor.width, descriptor.height);
		//glTextureSubImage2D(object, 0, 0, 0, descriptor.width, descriptor.height, GL_RGBA, GL_UNSIGNED_BYTE, descriptor.data);

		glTextureParameteri(object, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(object, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(object, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(object, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture::~OpenGLTexture() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~OpenGLTexture]");

		glDeleteTextures(1, &object);
	}

	uint32 OpenGLTexture::getWidth() const {
		return width;
	}

	uint32 OpenGLTexture::getHeight() const {
		return height;
	}

	/*void OpenGLTexture::setData(const void* data, uint32 size) const {
		PROFILE_SCOPE(FUNC_SIG);

		// TODO: Store channel count?
		const uint32 channelCount = format == GL_RGBA ? 4 : 3;

		const bool ok = size == (width * height * channelCount);

		ENGINE_ASSERT(ok, "Data must be entire texture.");

		glTextureSubImage2D(object, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
	}*/
}