#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLInstance.hpp"

namespace ProjectA {
	OpenGLInstance::OpenGLInstance() {
		PROFILE_SCOPE(FUNCSIG);

		#ifdef DEBUG_ENABLE_MESSENGER
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(debugMessageCallback, nullptr);
		#endif
	}

	#ifdef DEBUG_ENABLE_MESSENGER
		void OpenGLInstance::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			static constexpr std::string_view log = "[] []: [].";
			static constexpr char* sourceStrings[] = {
				"OpenGL API",
				"Window-system API",
				"Shader compiler",
				"Third-party",
				"Application",
				"Unknown source",
			};

			const char* sourceString = sourceStrings[source - GL_DEBUG_SOURCE_API];
			std::string_view typeString;

			switch (type) {
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
					typeString = "deprecation warning";

					break;
				case GL_DEBUG_TYPE_ERROR:
					typeString = "error";

					break;
				case GL_DEBUG_TYPE_MARKER:
					typeString = "command stream annotation";

					break;
				case GL_DEBUG_TYPE_OTHER:
					typeString = "message";

					break;
				case GL_DEBUG_TYPE_PERFORMANCE:
					typeString = "performance warning";

					break;
				case GL_DEBUG_TYPE_POP_GROUP:
					typeString = "group popping";

					break;
				case GL_DEBUG_TYPE_PORTABILITY:
					typeString = "portability warning";

					break;
				case GL_DEBUG_TYPE_PUSH_GROUP:
					typeString = "group pushing";

					break;
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
					typeString = "undefined behavior warning";

					break;
			}

			switch (severity) {
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					ENGINE_INFO(log, sourceString, typeString, message);

					break;
				case GL_DEBUG_SEVERITY_LOW:
				case GL_DEBUG_SEVERITY_MEDIUM:
					ENGINE_WARN(log, sourceString, typeString, message);

					break;
				case GL_DEBUG_SEVERITY_HIGH:
					ENGINE_ERROR(log, sourceString, typeString, message);

					break;
			}
		}
	#endif
}