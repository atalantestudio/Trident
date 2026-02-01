// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/GL/ShaderModule.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::ComputePipeline::ComputePipeline(const ComputePipelineDescriptor& descriptor) :
		Trident::ComputePipeline(descriptor),
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

			ADAPTER_ASSERT(false, "Program linking failed:\n[]", infoLog);

			return;
		}

		glDetachShader(program, computeShaderModule->shader);

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				glObjectLabel(GL_PROGRAM, program, static_cast<GLsizei>(descriptor.name.count()), &descriptor.name[0]);
			}
		#endif
	}

	GL::ComputePipeline::~ComputePipeline() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteProgram(program);
	}
}