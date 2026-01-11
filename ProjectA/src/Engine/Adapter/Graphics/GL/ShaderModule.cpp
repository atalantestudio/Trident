// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/GL/ShaderStage.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	GL::ShaderModule::ShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) :
		ProjectA::ShaderModule(descriptor),
		shader(glCreateShader(getShaderStage(descriptor.stage)))
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ADAPTER_ASSERT(descriptor.stage == ShaderStage::VERTEX, "Only vertex shaders can be loaded as .tsm assets.");

		#ifdef GL_SOURCE_FROM_SPIRV
			// TODO
			ENGINE_ERROR_UNIMPLEMENTED();
		#else
			sequence<char8> code = asset->getGlslCode();

			const std::unordered_map<sequence<char8>, uint32>& entryPointActivationOffsets = asset->getGlslEntryPointActivationOffsets();
			const std::unordered_map<sequence<char8>, uint32>::const_iterator entryPointActivationOffsetEntry = find(entryPointActivationOffsets, descriptor.entryPoint);

			ADAPTER_ASSERT(entryPointActivationOffsetEntry != entryPointActivationOffsets.end(), "Entry point \"[]\" is not present in the shader module.", descriptor.entryPoint);

			code[entryPointActivationOffsetEntry->second] = '1';

			const GLchar* const string = static_cast<const GLchar* const>(&code[0]);
			const GLsizei length = static_cast<GLsizei>(code.count());

			glShaderSource(shader, 1, &string, &length);
			glCompileShader(shader);
		#endif

		checkCompileStatus();
	}

	GL::ShaderModule::ShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) :
		ProjectA::ShaderModule(descriptor),
		shader(glCreateShader(getShaderStage(descriptor.stage)))
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GLsizei length = static_cast<GLsizei>(code.count());

		#ifdef GL_SOURCE_FROM_SPIRV
			#if OPENGL_VERSION >= OPENGL_VERSION_4_6
				glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, &code[0], length);

				// TODO: Expose specialization constants.
				glSpecializeShader(shader, &descriptor.entryPoint[0], 0, nullptr, nullptr);
			#else
				ENGINE_ERROR_UNSUPPORTED();
			#endif
		#else
			std::string codeString(code.begin(), code.end());

			if (descriptor.entryPoint != "main") {
				static const std::regex postVersionDirective("#version\\s+\\d+");

				std::smatch matchResults;

				const bool match = std::regex_search(codeString, matchResults, postVersionDirective);

				ADAPTER_ASSERT(match, "Could not find #version directive in GLSL string.");

				const std::size_t offset = matchResults[0].length();

				std::string entryPointDefine;
				entryPointDefine += "\n#define ";
				entryPointDefine += descriptor.entryPoint;
				entryPointDefine += " main";

				codeString.replace(offset, 0, entryPointDefine);

				length = static_cast<GLsizei>(codeString.size());
			}

			const GLchar* const string = static_cast<const GLchar* const>(codeString.data());

			glShaderSource(shader, 1, &string, &length);
			glCompileShader(shader);
		#endif

		checkCompileStatus();
	}

	GL::ShaderModule::~ShaderModule() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteShader(shader);
	}

	void GL::ShaderModule::checkCompileStatus() const {
		GLint compileStatus;

		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == GL_TRUE) {
			return;
		}

		GLint infoLogLength;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		sequence<GLchar> infoLog(infoLogLength);

		glGetShaderInfoLog(shader, infoLogLength, nullptr, &infoLog[0]);
		glDeleteShader(shader);

		ADAPTER_ASSERT(false, "Shader module compilation failed:\n[]", infoLog);
	}
}