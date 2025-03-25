#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLInstance.hpp"

namespace ProjectA {
	OpenGLInstance::OpenGLInstance() {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLInstance]");

		#ifdef DEBUG_ENABLE_MESSENGER
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(debugMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		#endif
	}

	#ifdef DEBUG_ENABLE_MESSENGER
		void OpenGLInstance::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			switch (severity) {
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					ENGINE_INFO("[] (source: [], type: [], id: [])", message, source, type, id);

					break;
				case GL_DEBUG_SEVERITY_LOW:
					ENGINE_WARN("[] (source: [], type: [], id: [])", message, source, type, id);

					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
				case GL_DEBUG_SEVERITY_HIGH:
					ENGINE_ERROR("[] (source: [], type: [], id: [])", message, source, type, id);

					break;
			}
		}
	#endif
}