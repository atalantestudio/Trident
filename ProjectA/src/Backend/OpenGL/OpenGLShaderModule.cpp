#include "pch.hpp"

#include "Backend/OpenGL/OpenGLShaderModule.hpp"
#include "Backend/OpenGL/ShaderStage.hpp"

namespace ProjectA {
	OpenGLShaderModule::OpenGLShaderModule(const ShaderModuleDescriptor& descriptor, const chain<uint8>& code) :
		ObjectWrapper(glCreateShader(getGlShaderStage(descriptor.stage)))
	{
		PROFILE_SCOPE(FUNC_SIG);

		const GLsizei length = static_cast<GLsizei>(code.size());

		glShaderBinary(1, &object, GL_SHADER_BINARY_FORMAT_SPIR_V, code.data(), length);

		// TODO: Expose specialization constants.
		glSpecializeShader(object, descriptor.entryPoint, 0, nullptr, nullptr);

		GLint compileStatus;

		glGetShaderiv(object, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus != GL_TRUE) {
			GLint infoLogLength;

			glGetShaderiv(object, GL_INFO_LOG_LENGTH, &infoLogLength);

			chain<GLchar> infoLog(infoLogLength);

			glGetShaderInfoLog(object, infoLogLength, nullptr, infoLog.data());

			glDeleteShader(object);

			ENGINE_ERROR("Shader module compilation failed:\n[]", infoLog.data());

			throw std::runtime_error("Failed to create OpenGL shader module.");
		}
	}

	OpenGLShaderModule::~OpenGLShaderModule() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteShader(object);
	}
}