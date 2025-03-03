#include "pch.hpp"

#include "Platform/OpenGL/OpenGLShaderModule.hpp"

namespace ProjectA {
	OpenGLShaderModule::OpenGLShaderModule(const GLenum stage, const GLchar* const code) :
		ObjectWrapper(glCreateShader(stage))
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[OpenGLShaderModule]");

		glShaderSource(object, 1, &code, nullptr);
		glCompileShader(object);

		GLint compileStatus;

		glGetShaderiv(object, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == 0) {
			GLint infoLogLength;

			glGetShaderiv(object, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::vector<GLchar> infoLog(infoLogLength);

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