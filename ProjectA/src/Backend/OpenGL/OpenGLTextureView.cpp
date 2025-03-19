#include "pch.hpp"

#include "Backend/OpenGL/OpenGLTextureView.hpp"
#include "Backend/OpenGL/TextureType.hpp"

namespace ProjectA {
	OpenGLTextureView::OpenGLTextureView(const OpenGLTexture& texture, const TextureViewDescriptor& descriptor) :
		texture(texture.object)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[OpenGLTextureView]");

		// glGenTextures must be used here because the view must not be assigned a target.
		glGenTextures(1, &object);

		glTextureView(object, texture.target, texture.object, GL_RGBA8, 0, 1, 0, 1);
	}

	OpenGLTextureView::~OpenGLTextureView() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~OpenGLTextureView]");

		glDeleteTextures(1, &object);
	}
}