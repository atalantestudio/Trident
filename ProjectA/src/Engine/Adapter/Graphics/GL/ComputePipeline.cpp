#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/GL/ShaderModule.hpp"

namespace ProjectA {
	OpenGLComputePipeline::OpenGLComputePipeline(const ComputePipelineDescriptor& descriptor) :
		ComputePipeline(descriptor),
		program(glCreateProgram())
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const ref<OpenGLShaderModule> computeShaderModule = std::static_pointer_cast<OpenGLShaderModule>(descriptor.computeShaderModule);

		glAttachShader(program, computeShaderModule->object);
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

		glDetachShader(program, computeShaderModule->object);
	}

	OpenGLComputePipeline::~OpenGLComputePipeline() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteProgram(program);
	}
}