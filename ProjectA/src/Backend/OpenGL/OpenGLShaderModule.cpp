#include "pch.hpp"

#include "Backend/OpenGL/OpenGLShaderModule.hpp"

#include "Backend/OpenGL/ShaderStage.hpp"

#include "Engine/config.hpp"

namespace ProjectA {
	OpenGLShaderModule::OpenGLShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) :
		ObjectWrapper(glCreateShader(getGlShaderStage(descriptor.stage)))
	{
		PROFILE_SCOPE(FUNC_SIG);

		const GLsizei length = static_cast<GLsizei>(code.count);

		#ifdef OPENGL_USE_SPIRV
			glShaderBinary(1, &object, GL_SHADER_BINARY_FORMAT_SPIR_V, code, length);

			// TODO: Expose specialization constants.
			glSpecializeShader(object, descriptor.entryPoint.data(), 0, nullptr, nullptr);
		#else
			const GLchar* const string = static_cast<const GLchar* const>(static_cast<const void* const>(code));

			// TODO: Handle custom entry point name.
			// #define TheActualEntrypoint main
			glShaderSource(object, 1, &string, &length);
			glCompileShader(object);
		#endif

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