#include "pch.hpp"

#include "Backend/OpenGL/OpenGLTextureView.hpp"

namespace ProjectA {
	OpenGLTextureView::OpenGLTextureView(const TextureViewDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[OpenGLTextureView]");
	}

	OpenGLTextureView::~OpenGLTextureView() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~OpenGLTextureView]");
	}
}