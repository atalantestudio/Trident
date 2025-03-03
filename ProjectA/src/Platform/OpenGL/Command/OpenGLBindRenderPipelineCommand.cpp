#include "pch.hpp"

#include <glad/glad.h>

#include "Platform/OpenGL/Command/OpenGLBindRenderPipelineCommand.hpp"

namespace ProjectA {
	OpenGLBindRenderPipelineCommand::OpenGLBindRenderPipelineCommand(const OpenGLRenderPipeline* renderPipeline) :
		renderPipeline(renderPipeline)
	{}

	void OpenGLBindRenderPipelineCommand::execute() const {
		glUseProgram(renderPipeline->program);
	}
}