#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"

namespace ProjectA {
	class OpenGLBindRenderPipelineCommand final : public OpenGLCommand {
		public:
			explicit OpenGLBindRenderPipelineCommand(const ref<OpenGLRenderPipeline>& renderPipeline) :
				renderPipeline(renderPipeline)
			{}

			// TODO(review): Make glEnable/glDisable calls when initializing?
			void execute() const final override {
				DEBUG_ASSERT(renderPipeline->minSampleShading >= 0.0f && renderPipeline->minSampleShading <= 1.0f);

				glBindVertexArray(renderPipeline->vertexArray);
				glUseProgram(renderPipeline->program);
				glFrontFace(renderPipeline->frontFace);

				if (renderPipeline->polygonModeFace != GL_NONE) {
					glPolygonMode(GL_FRONT_AND_BACK, renderPipeline->polygonMode);
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

				if (!renderPipeline->dynamicLineWidth) {
					glLineWidth(renderPipeline->lineWidth);
				}

				if (renderPipeline->minSampleShading != 0.0f) {
					glEnable(GL_SAMPLE_SHADING);
					glMinSampleShading(renderPipeline->minSampleShading);
				}
				else {
					glDisable(GL_SAMPLE_SHADING);
				}

				for (uint32 drawBufferIndex = 0; drawBufferIndex < renderPipeline->drawBuffers.size(); drawBufferIndex += 1) {
					const OpenGLDrawBuffer& drawBuffer = renderPipeline->drawBuffers[drawBufferIndex];

					if (drawBuffer.blend) {
						glEnablei(GL_BLEND, drawBufferIndex);
						glBlendFunci(drawBufferIndex, drawBuffer.srcColorBlendFactor, drawBuffer.dstColorBlendFactor);
					}
					else {
						glDisablei(GL_BLEND, drawBufferIndex);
					}
				}

				if (renderPipeline->depthTest) {
					glEnable(GL_DEPTH_TEST);
					glDepthMask(renderPipeline->depthWrite);
					glDepthFunc(renderPipeline->depthFunc);
				}
				else {
					glDisable(GL_DEPTH_TEST);
				}

				// TODO: Handle SRGB with GL_FRAMEBUFFER_SRGB.
			}

		private:
			const ref<OpenGLRenderPipeline> renderPipeline;
	};
}