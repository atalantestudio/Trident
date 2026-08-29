// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/GraphicsPipeline.hpp"

namespace Trident {
	class GL::BindGraphicsPipelineCommand final : public GL::Command {
		public:
			explicit BindGraphicsPipelineCommand(ref<GL::GraphicsPipeline> graphicsPipeline) :
				graphicsPipeline(graphicsPipeline)
			{}

			void execute() const final override {
				glBindVertexArray(graphicsPipeline->vertexArray);
				glUseProgram(graphicsPipeline->program);
				glFrontFace(graphicsPipeline->frontFace);

				if (graphicsPipeline->polygonModeFace != GL_NONE) {
					glPolygonMode(GL_FRONT_AND_BACK, graphicsPipeline->polygonMode);
				}

				if (graphicsPipeline->polygonMode == GL_POINT) {
					glEnable(GL_PROGRAM_POINT_SIZE);
				}
				else {
					glDisable(GL_PROGRAM_POINT_SIZE);
				}

				if (graphicsPipeline->cullMode != 0) {
					glEnable(GL_CULL_FACE);
					glCullFace(graphicsPipeline->cullMode);
				}
				else {
					glDisable(GL_CULL_FACE);
				}

				if (!graphicsPipeline->dynamicLineWidth) {
					glLineWidth(graphicsPipeline->lineWidth);
				}

				if (graphicsPipeline->multisample) {
					glEnable(GL_MULTISAMPLE);

					if (graphicsPipeline->minSampleShading != 0.0f) {
						glEnable(GL_SAMPLE_SHADING);

						#if OPENGL_VERSION >= OPENGL_VERSION_4_0
							glMinSampleShading(graphicsPipeline->minSampleShading);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else {
						glDisable(GL_SAMPLE_SHADING);
					}
				}
				else {
					glDisable(GL_MULTISAMPLE);
				}

				uint32 drawBufferIndex = 0;

				while (drawBufferIndex < graphicsPipeline->drawBuffers.count()) {
					const GL::GraphicsPipeline::DrawBuffer drawBuffer = graphicsPipeline->drawBuffers[drawBufferIndex];

					if (drawBuffer.blend) {
						glEnablei(GL_BLEND, drawBufferIndex);

						#if OPENGL_VERSION >= OPENGL_VERSION_4_0
							glBlendFunci(drawBufferIndex, drawBuffer.srcColorBlendFactor, drawBuffer.dstColorBlendFactor);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else {
						glDisablei(GL_BLEND, drawBufferIndex);
					}

					drawBufferIndex += 1;
				}

				if (graphicsPipeline->depthTest) {
					glEnable(GL_DEPTH_TEST);
					glDepthMask(graphicsPipeline->depthWrite);
					glDepthFunc(graphicsPipeline->depthFunc);
				}
				else {
					glDisable(GL_DEPTH_TEST);
				}

				// TODO(graphics/gl/srgb-framebuffer): Handle SRGB with GL_FRAMEBUFFER_SRGB.
			}

		private:
			ref<GL::GraphicsPipeline> graphicsPipeline;
	};
}