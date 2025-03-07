#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLRenderPass.hpp"

namespace ProjectA {
	OpenGLRenderPass::OpenGLRenderPass(const RenderPassDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLRenderPass]");

		// TODO: Expose.
		//glFrontFace(GL_CW);
		//glCullFace(GL_BACK);
		//glEnable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}