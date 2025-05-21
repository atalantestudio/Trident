#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	class OpenGLEndRenderPassCommand : public OpenGLCommand {
		public:
			OpenGLEndRenderPassCommand(const OpenGLRenderPass& renderPass, const OpenGLSwapchain& swapchain) :
				renderPass(renderPass),
				swapchain(swapchain)
			{}

			void execute() const override {
				DEBUG_ASSERT(swapchain.framebuffers.size() == 1);

				if (swapchain.requiresResolvePass()) {
					glBindFramebuffer(GL_READ_FRAMEBUFFER, swapchain.framebuffers[0]);
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

					glReadBuffer(GL_COLOR_ATTACHMENT0);
					glDrawBuffer(GL_FRONT);

					glBlitFramebuffer(
						0,
						0,
						swapchain.getExtentWidth(),
						swapchain.getExtentHeight(),
						0,
						0,
						swapchain.getExtentWidth(),
						swapchain.getExtentHeight(),
						GL_COLOR_BUFFER_BIT,
						GL_NEAREST
					);

					//glBindFramebuffer(GL_FRAMEBUFFER, 0);

					// The default framebuffer has already been cleared in the render pass begin command.

					//glUseProgram(renderPass.screenProgram);
					//glBindTextureUnit(0, swapchain.images[0]->object);
					//glDrawArrays(GL_TRIANGLES, 0, 3);
				}
			}

		private:
			const OpenGLRenderPass& renderPass;
			const OpenGLSwapchain& swapchain;
	};
}