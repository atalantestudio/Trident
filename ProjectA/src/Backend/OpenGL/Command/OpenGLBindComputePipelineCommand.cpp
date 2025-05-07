#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBindComputePipelineCommand.hpp"
#include "Backend/OpenGL/DescriptorType.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"

namespace ProjectA {
	OpenGLBindComputePipelineCommand::OpenGLBindComputePipelineCommand(const ref<OpenGLComputePipeline>& computePipeline) :
		computePipeline(computePipeline)
	{}

	void OpenGLBindComputePipelineCommand::execute() const {
		glUseProgram(computePipeline->program);

		// TODO: Move this in CommandEncoder::bindAllDescriptorSets.
		computePipeline->layout.bind();
	}
}