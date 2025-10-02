#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"

namespace ProjectA {
	class OpenGLBindRenderPipelineCommand final : public OpenGLCommand {
		public:
			explicit OpenGLBindRenderPipelineCommand(const ref<OpenGLRenderPipeline>& renderPipeline) :
				renderPipeline(renderPipeline)
			{}

			void execute() const final override {
				DEBUG_ASSERT(renderPipeline->minSampleShading >= 0.0f && renderPipeline->minSampleShading <= 1.0f);

				#if OPENGL_VERSION >= OPENGL_VERSION_3_0
					glBindVertexArray(renderPipeline->vaobj);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif

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

					#if OPENGL_VERSION >= OPENGL_VERSION_4_0
						glMinSampleShading(renderPipeline->minSampleShading);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}
				else {
					glDisable(GL_SAMPLE_SHADING);
				}

				for (uint32 drawBufferIndex = 0; drawBufferIndex < renderPipeline->drawBuffers.count; drawBufferIndex += 1) {
					const OpenGLDrawBuffer& drawBuffer = renderPipeline->drawBuffers[drawBufferIndex];

					if (drawBuffer.blend) {
						#if OPENGL_VERSION >= OPENGL_VERSION_3_0
							glEnablei(GL_BLEND, drawBufferIndex);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif

						#if OPENGL_VERSION >= OPENGL_VERSION_4_0
							glBlendFunci(drawBufferIndex, drawBuffer.srcColorBlendFactor, drawBuffer.dstColorBlendFactor);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else {
						#if OPENGL_VERSION >= OPENGL_VERSION_3_0
							glDisablei(GL_BLEND, drawBufferIndex);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
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