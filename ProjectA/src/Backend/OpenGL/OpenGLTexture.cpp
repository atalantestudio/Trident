#include "pch.hpp"

#include "Backend/OpenGL/OpenGLTexture.hpp"
#include "Backend/OpenGL/OpenGLTextureView.hpp"
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

	ref<TextureView> OpenGLTexture::createView(const TextureViewDescriptor& descriptor) const {
		return createRef<OpenGLTextureView>(descriptor);
	}
}