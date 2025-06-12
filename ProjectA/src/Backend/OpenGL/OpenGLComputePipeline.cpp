#include "pch.hpp"

#include "Backend/OpenGL/OpenGLComputePipeline.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"

namespace ProjectA {
	OpenGLComputePipeline::OpenGLComputePipeline(const ComputePipelineDescriptor& descriptor) :
		layout(*std::static_pointer_cast<OpenGLPipelineLayout>(descriptor.pipelineLayout)),
		program(glCreateProgram())
	{
		PROFILE_SCOPE(FUNC_SIG);

		const ref<OpenGLShaderModule> computeShaderModule = std::static_pointer_cast<OpenGLShaderModule>(descriptor.computeShaderModule);

		glAttachShader(program, computeShaderModule->object);
		glLinkProgram(program);

		GLint linkStatus;

		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus != GL_TRUE) {
			GLint infoLogLength;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			sequence<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog);

			ENGINE_ERROR("Program linking failed:\n[]", infoLog);

			BREAKPOINT();
		}

		glDetachShader(program, computeShaderModule->object);
	}

	OpenGLComputePipeline::~OpenGLComputePipeline() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteProgram(program);
	}
}