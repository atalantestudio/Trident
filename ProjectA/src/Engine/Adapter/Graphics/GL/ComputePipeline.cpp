// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/GL/ShaderModule.hpp"

namespace ProjectA {
	GL::ComputePipeline::ComputePipeline(const ComputePipelineDescriptor& descriptor) :
		ProjectA::ComputePipeline(descriptor),
		program(glCreateProgram())
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const ref<GL::ShaderModule> computeShaderModule = std::static_pointer_cast<GL::ShaderModule>(descriptor.computeShaderModule);

		glAttachShader(program, computeShaderModule->shader);
		glLinkProgram(program);

		GLint linkStatus;

		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus != GL_TRUE) {
			GLint infoLogLength;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			sequence<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(program, infoLogLength, nullptr, &infoLog[0]);

			ENGINE_ERROR("Program linking failed:\n[]", infoLog);

			DEBUG_ASSERT(false);

			return;
		}

		glDetachShader(program, computeShaderModule->shader);
	}

	GL::ComputePipeline::~ComputePipeline() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteProgram(program);
	}
}