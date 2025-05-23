#include "pch.hpp"

#include "Backend/OpenGL/OpenGLShaderModule.hpp"

#include "Backend/OpenGL/ShaderStage.hpp"

namespace ProjectA {
	OpenGLShaderModule::OpenGLShaderModule(const std::vector<uint8>& code, ShaderStage stage) :
		ObjectWrapper(glCreateShader(getGlShaderStage(stage)))
	{
		PROFILE_SCOPE(FUNC_SIG);

		const GLchar* const string = reinterpret_cast<const GLchar* const>(code.data());
		const GLint length = static_cast<GLint>(code.size());

		glShaderSource(object, 1, &string, &length);
		glCompileShader(object);

		checkCompileStatus();
	}

	OpenGLShaderModule::OpenGLShaderModule(const std::vector<uint8>& code, const ShaderModuleDescriptor& descriptor) :
		ObjectWrapper(glCreateShader(getGlShaderStage(descriptor.stage)))
	{
		PROFILE_SCOPE(FUNC_SIG);

		const GLsizei length = static_cast<GLsizei>(code.size());

		glShaderBinary(1, &object, GL_SHADER_BINARY_FORMAT_SPIR_V, code.data(), length);

		// TODO: Expose specialization constants.
		glSpecializeShader(object, descriptor.entryPoint.data(), 0, nullptr, nullptr);

		checkCompileStatus();
	}

	OpenGLShaderModule::~OpenGLShaderModule() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteShader(object);
	}

	void OpenGLShaderModule::checkCompileStatus() const {
		GLint compileStatus;

		glGetShaderiv(object, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == GL_TRUE) {
			return;
		}

		GLint infoLogLength;

		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &infoLogLength);

		std::vector<GLchar> infoLog(infoLogLength);

		glGetShaderInfoLog(object, infoLogLength, nullptr, infoLog.data());
		glDeleteShader(object);

		ENGINE_ERROR("Shader module compilation failed:\n[]", infoLog.data());
		BREAKPOINT();
	}
}