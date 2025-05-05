#include "pch.hpp"

#include "Backend/OpenGL/OpenGLFence.hpp"

namespace ProjectA {
	void OpenGLFence::create() {
		PROFILE_SCOPE(FUNC_SIG);

		// See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFenceSync.xhtml.
		object = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		ENGINE_ASSERT(object != 0, "Failed to create OpenGL fence.");
	}

	void OpenGLFence::reset() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteSync(object);
	}
}