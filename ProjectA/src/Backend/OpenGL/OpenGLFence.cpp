#include "pch.hpp"

#include "Backend/OpenGL/OpenGLFence.hpp"

namespace ProjectA {
	void OpenGLFence::create() {
		PROFILE_SCOPE(FUNCSIG);

		object = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

	void OpenGLFence::reset() {
		PROFILE_SCOPE(FUNCSIG);

		glDeleteSync(object);
	}
}