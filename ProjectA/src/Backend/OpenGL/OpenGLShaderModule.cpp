#include "pch.hpp"

#include "Backend/OpenGL/OpenGLShaderModule.hpp"

namespace ProjectA {
	OpenGLShaderModule::OpenGLShaderModule(const GLenum stage, const chain<uint8>& code) :
		ObjectWrapper(glCreateShader(stage))
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[OpenGLShaderModule]");

		const GLchar* const string = reinterpret_cast<const GLchar* const>(code.data());
		const GLint length = static_cast<GLint>(code.size());

		glShaderSource(object, 1, &string, &length);
		glCompileShader(object);

		GLint compileStatus;

		glGetShaderiv(object, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == 0) {
			GLint infoLogLength;

			glGetShaderiv(object, GL_INFO_LOG_LENGTH, &infoLogLength);

			chain<GLchar> infoLog(infoLogLength);

			glGetShaderInfoLog(object, infoLogLength, nullptr, infoLog.data());

			ENGINE_ERROR("Shader module compilation failed.\n[]", infoLog.data());

			throw std::runtime_error("Failed to create OpenGL shader module.");
		}
	}

	OpenGLShaderModule::~OpenGLShaderModule() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~OpenGLShaderModule]");

		glDeleteShader(object);
	}
}