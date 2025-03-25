#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/Instance.hpp"

namespace ProjectA {
	class OpenGLInstance : public Instance {
		public:
			OpenGLInstance();

		private:
			#ifdef DEBUG_ENABLE_MESSENGER
				static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
			#endif
	};
}