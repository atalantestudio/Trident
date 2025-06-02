#include "pch.hpp"

#include "Backend/OpenGL/OpenGLFence.hpp"

namespace ProjectA {
	OpenGLFence::OpenGLFence() :
		ObjectWrapper(nullptr)
	{}

	void OpenGLFence::create() {
		PROFILE_SCOPE(FUNC_SIG);

		// See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFenceSync.xhtml.
		object = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		ENGINE_ASSERT(object != 0, "Failed to create OpenGL fence.");
	}

	void OpenGLFence::reset() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteSync(object);

		object = nullptr;
	}

	bool OpenGLFence::isSignaled() const {
		if (object == nullptr) {
			return false;
		}

		GLint value;

		glGetSynciv(object, GL_SYNC_STATUS, 1, nullptr, &value);

		return static_cast<bool>(value - GL_UNSIGNALED);
	}
}