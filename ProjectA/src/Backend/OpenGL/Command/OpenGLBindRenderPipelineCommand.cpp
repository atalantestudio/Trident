#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBindRenderPipelineCommand.hpp"

namespace ProjectA {
	OpenGLBindRenderPipelineCommand::OpenGLBindRenderPipelineCommand(const ref<OpenGLRenderPipeline>& renderPipeline) :
		renderPipeline(renderPipeline)
	{}

	void OpenGLBindRenderPipelineCommand::execute() const {
		// TODO: Move into OpenGLPipeline::bind.

		glBindVertexArray(renderPipeline->vertexArray);

		glUseProgram(renderPipeline->program);

		glFrontFace(renderPipeline->frontFace);

		if (renderPipeline->cullMode == 0) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(renderPipeline->cullMode);
		}

		glEnable(GL_FRAMEBUFFER_SRGB);

		// TODO: Expose.
		//glEnable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}