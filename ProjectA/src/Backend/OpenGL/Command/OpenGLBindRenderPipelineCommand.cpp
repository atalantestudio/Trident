#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBindRenderPipelineCommand.hpp"

namespace ProjectA {
	OpenGLBindRenderPipelineCommand::OpenGLBindRenderPipelineCommand(const ref<OpenGLRenderPipeline>& renderPipeline) :
		renderPipeline(renderPipeline)
	{}

	// TODO: Expose:
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

		if (!renderPipeline->dynamicLineWidth) {
			glLineWidth(renderPipeline->lineWidth);
		}

		if (renderPipeline->polygonMode == GL_POINT) {
			glEnable(GL_PROGRAM_POINT_SIZE);
		}
		else {
			glDisable(GL_PROGRAM_POINT_SIZE);
		}

		if (renderPipeline->cullMode != 0) {
			glEnable(GL_CULL_FACE);
			glCullFace(renderPipeline->cullMode);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		if (renderPipeline->minSampleShading != 0.0f) {
			glEnable(GL_SAMPLE_SHADING);
			glMinSampleShading(renderPipeline->minSampleShading);
		}
		else {
			glDisable(GL_SAMPLE_SHADING);
		}
	}
}