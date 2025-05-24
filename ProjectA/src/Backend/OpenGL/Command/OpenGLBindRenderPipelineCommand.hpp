#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"

namespace ProjectA {
	class OpenGLBindRenderPipelineCommand : public OpenGLCommand {
		public:
			OpenGLBindRenderPipelineCommand(const ref<OpenGLRenderPipeline>& renderPipeline) :
				renderPipeline(renderPipeline)
			{}

			// TODO: glEnable(GL_BLEND)
			// TODO: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
			// TODO: glEnable(GL_FRAMEBUFFER_SRGB)
			void execute() const override {
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

				if (renderPipeline->depthTest) {
					glEnable(GL_DEPTH_TEST);
					glDepthMask(renderPipeline->depthWrite);
					glDepthFunc(renderPipeline->depthFunc);
				}
				else {
					glDisable(GL_DEPTH_TEST);
				}
			}

		private:
			const ref<OpenGLRenderPipeline> renderPipeline;
	};
}