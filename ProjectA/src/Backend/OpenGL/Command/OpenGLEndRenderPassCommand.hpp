#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLGraphicsAdapter.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"

namespace ProjectA {
	class OpenGLEndRenderPassCommand final : public OpenGLCommand {
		public:
			explicit OpenGLEndRenderPassCommand(const ref<OpenGLFramebuffer>& framebuffer) :
				framebuffer(framebuffer)
			{}

			void execute() const final override {
				const ref<OpenGLRenderPass>& renderPass = std::static_pointer_cast<OpenGLRenderPass>(framebuffer->getRenderPass());
				const uint32 presentSourceAttachmentIndex = renderPass->getPresentSourceAttachmentIndex();

				if (presentSourceAttachmentIndex == max<uint32>()) {
					return;
				}

				const GLenum readBuffer = GL_COLOR_ATTACHMENT0 + presentSourceAttachmentIndex;
				const Extent2D extent = framebuffer->getExtent();

				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glNamedFramebufferReadBuffer(framebuffer->object, readBuffer);

					glBlitNamedFramebuffer(
						framebuffer->object,
						0,
						0,
						0,
						extent.width,
						extent.height,
						0,
						0,
						extent.width,
						extent.height,
						GL_COLOR_BUFFER_BIT,
						GL_NEAREST
					);
				#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
					glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->object);
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

					glReadBuffer(readBuffer);
					glDrawBuffer(OpenGLGraphicsAdapter::defaultFramebufferDrawBuffer);

					glBlitFramebuffer(
						0,
						0,
						extent.width,
						extent.height,
						0,
						0,
						extent.width,
						extent.height,
						GL_COLOR_BUFFER_BIT,
						GL_NEAREST
					);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			const ref<OpenGLFramebuffer> framebuffer;
	};
}