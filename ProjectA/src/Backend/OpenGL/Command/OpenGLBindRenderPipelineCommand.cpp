#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBindRenderPipelineCommand.hpp"

namespace ProjectA {
	OpenGLBindRenderPipelineCommand::OpenGLBindRenderPipelineCommand(const OpenGLRenderPipeline* renderPipeline) :
		renderPipeline(renderPipeline)
	{}

	void OpenGLBindRenderPipelineCommand::execute() const {
		glUseProgram(renderPipeline->program);

		glFrontFace(renderPipeline->frontFace);

		if (renderPipeline->cullMode == 0) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(renderPipeline->cullMode);
		}
	}
}