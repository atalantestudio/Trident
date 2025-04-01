#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBindComputePipelineCommand.hpp"

namespace ProjectA {
	OpenGLBindComputePipelineCommand::OpenGLBindComputePipelineCommand(const ref<OpenGLComputePipeline>& computePipeline) :
		computePipeline(computePipeline)
	{}

	void OpenGLBindComputePipelineCommand::execute() const {
		glUseProgram(computePipeline->program);
	}
}