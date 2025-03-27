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

		glCreateTextures(target, 1, &object);

		glTextureStorage2D(object, descriptor.mipLevelCount, GL_RGBA8, descriptor.width, descriptor.height);
	}

	OpenGLTexture::~OpenGLTexture() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteTextures(1, &object);
	}

	uint32 OpenGLTexture::getWidth() const {
		return width;
	}

	uint32 OpenGLTexture::getHeight() const {
		return height;
	}

	ref<TextureView> OpenGLTexture::createView(const TextureViewDescriptor& descriptor) const {
		return createRef<OpenGLTextureView>(*this, descriptor);
	}
}