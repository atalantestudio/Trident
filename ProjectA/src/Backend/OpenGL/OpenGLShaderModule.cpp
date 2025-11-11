#include "pch.hpp"

#include "Backend/OpenGL/OpenGLShaderModule.hpp"

#include "Backend/OpenGL/ShaderStage.hpp"

namespace ProjectA {
	OpenGLShaderModule::OpenGLShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) :
		ShaderModule(descriptor),
		ObjectWrapper(glCreateShader(getGlShaderStage(descriptor.stage)))
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GLsizei length = static_cast<GLsizei>(code.count());

		#ifdef OPENGL_USE_SPIRV
			#if OPENGL_VERSION >= OPENGL_VERSION_4_6
				glShaderBinary(1, &object, GL_SHADER_BINARY_FORMAT_SPIR_V, &code[0], length);

				// TODO: Expose specialization constants.
				glSpecializeShader(object, descriptor.entryPoint.data(), 0, nullptr, nullptr);
			#else
				ENGINE_ERROR_UNSUPPORTED();
			#endif
		#else
			std::string codeString(code.begin(), code.end());

			if (descriptor.entryPoint != "main") {
				static const std::regex postVersionDirective("#version\\s+\\d+");

				std::smatch matchResults;

				const bool match = std::regex_search(codeString, matchResults, postVersionDirective);

				ENGINE_ASSERT(match, "Could not find #version directive in GLSL string.");

				const std::size_t offset = matchResults[0].length();

				std::string entryPointDefine;
				entryPointDefine += "\n#define ";
				entryPointDefine += descriptor.entryPoint;
				entryPointDefine += " main";

				codeString.replace(offset, 0, entryPointDefine);

				length = static_cast<GLsizei>(codeString.size());
			}

			const GLchar* const string = static_cast<const GLchar* const>(codeString.data());

			glShaderSource(object, 1, &string, &length);
			glCompileShader(object);
		#endif

		checkCompileStatus();
	}

	OpenGLShaderModule::~OpenGLShaderModule() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

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

		sequence<GLchar> infoLog(infoLogLength);

		glGetShaderInfoLog(object, infoLogLength, nullptr, &infoLog[0]);
		glDeleteShader(object);

		ENGINE_ERROR("Shader module compilation failed:\n[]", infoLog);

		DEBUG_ASSERT(false);
	}
}