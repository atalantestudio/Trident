#include "pch.hpp"

#include "Platform/OpenGL/Command/OpenGLBindRenderPipelineCommand.hpp"

namespace ProjectA {
	OpenGLBindRenderPipelineCommand::OpenGLBindRenderPipelineCommand(const OpenGLRenderPipeline* renderPipeline) :
		renderPipeline(renderPipeline)
	{}

	void OpenGLBindRenderPipelineCommand::execute() const {
		glUseProgram(renderPipeline->program);
	}
}