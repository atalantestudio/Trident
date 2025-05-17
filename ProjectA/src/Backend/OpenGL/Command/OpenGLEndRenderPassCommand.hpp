#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"

namespace ProjectA {
	class OpenGLEndRenderPassCommand : public OpenGLCommand {
		public:
			OpenGLEndRenderPassCommand(const OpenGLRenderPass& renderPass, const vec2<float32>& viewportOrigin, const vec2<float32>& viewportSize) :
				renderPass(renderPass),
				viewportOrigin(viewportOrigin),
				viewportSize(viewportSize)
			{}

			void execute() const override {
				if (renderPass.multisampleFramebuffer != 0) {
					glBindFramebuffer(GL_READ_FRAMEBUFFER, renderPass.multisampleFramebuffer);
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderPass.multisampleIntermediateFramebuffer);

					glBlitFramebuffer(
						static_cast<GLint>(viewportOrigin[0]),
						0,
						static_cast<GLint>(viewportSize[0]),
						lyah::abs(static_cast<GLint>(viewportSize[1])),
						static_cast<GLint>(viewportOrigin[0]),
						0,
						static_cast<GLint>(viewportSize[0]),
						lyah::abs(static_cast<GLint>(viewportSize[1])),
						GL_COLOR_BUFFER_BIT,
						GL_NEAREST
					);

					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					// The default framebuffer has already been cleared in the render pass begin command.

					glUseProgram(renderPass.multisampleProgram);
					glBindTextureUnit(0, renderPass.screenImage->object);
					glDrawArrays(GL_TRIANGLES, 0, 3);
				}
			}

		private:
			const OpenGLRenderPass& renderPass;
			const vec2<float32>& viewportOrigin;
			const vec2<float32>& viewportSize;
	};
}