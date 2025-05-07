#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBindRenderPipelineCommand.hpp"
#include "Backend/OpenGL/DescriptorType.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"

namespace ProjectA {
	OpenGLBindRenderPipelineCommand::OpenGLBindRenderPipelineCommand(const ref<OpenGLRenderPipeline>& renderPipeline) :
		renderPipeline(renderPipeline)
	{}

	// TODO: Expose these operations.
	// - glEnable(GL_BLEND)
	// - glEnable(GL_DEPTH_TEST)
	// - glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
	// - glEnable(GL_FRAMEBUFFER_SRGB)
	void OpenGLBindRenderPipelineCommand::execute() const {
		glBindVertexArray(renderPipeline->vertexArray);

		glUseProgram(renderPipeline->program);

		glFrontFace(renderPipeline->frontFace);

		if (renderPipeline->polygonModeFace != GL_NONE) {
			glPolygonMode(renderPipeline->polygonModeFace, renderPipeline->polygonMode);
		}

		if (renderPipeline->cullMode == 0) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(renderPipeline->cullMode);
		}

		// TODO: Move this in CommandEncoder::bindAllDescriptorSets.
		renderPipeline->layout.bind();
	}
}