#include "pch.hpp"

#include "Backend/OpenGL/OpenGLFence.hpp"

namespace ProjectA {
	OpenGLFence::OpenGLFence() :
		ObjectWrapper(nullptr)
	{}

	void OpenGLFence::create() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			// See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFenceSync.xhtml.
			object = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

			ENGINE_ASSERT(object != 0, "Failed to create OpenGL fence.");
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	void OpenGLFence::reset() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			glDeleteSync(object);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		object = nullptr;
	}
}