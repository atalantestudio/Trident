#include "pch.hpp"

#include "Platform/OpenGL/OpenGLShaderModule.hpp"

namespace ProjectA {
	OpenGLShaderModule::OpenGLShaderModule(const ShaderStage stage, const std::string& code) : ObjectWrapper(glCreateShader(shaderStages[static_cast<uint32>(stage)])) {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLchar* const string = code.c_str();

		glShaderSource(object, 1, &string, nullptr);
		glCompileShader(object);

		GLint compileStatus;

		glGetShaderiv(object, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == 0) {
			GLint infoLogLength;

			glGetShaderiv(object, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::vector<char> infoLog(infoLogLength);

			glGetShaderInfoLog(object, infoLogLength, nullptr, infoLog.data());

			ENGINE_ERROR("Shader compilation failed.\n[]", infoLog.data());

			this->~OpenGLShaderModule();

			// TODO: Throw instead of returning
			return;
		}

		ENGINE_TRACE("Compiled shader successfully.");
	}

	OpenGLShaderModule::~OpenGLShaderModule() {
		PROFILE_SCOPE(__FUNCSIG__);

		glDeleteShader(object);
	}
}