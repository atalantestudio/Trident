#include "pch.hpp"

#include "Backend/OpenGL/OpenGLFence.hpp"

namespace ProjectA {
	void OpenGLFence::create() {
		PROFILE_SCOPE(FUNC_SIG);

		object = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

	void OpenGLFence::reset() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteSync(object);
	}
}