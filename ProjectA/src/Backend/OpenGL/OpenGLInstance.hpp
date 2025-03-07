#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/Instance.hpp"

namespace ProjectA {
	class OpenGLInstance : public Instance {
		public:
			OpenGLInstance();

		private:
			#ifdef DEBUG_ENABLE_MESSENGER
				static void debugMessageCallback(const GLenum source, const GLenum type, const GLuint id, const GLenum severity, const GLsizei length, const GLchar* message, const void* userParam);
			#endif
	};
}