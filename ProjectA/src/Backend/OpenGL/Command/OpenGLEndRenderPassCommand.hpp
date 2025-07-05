#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLGraphicsAdapter.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"

namespace ProjectA {
	class OpenGLEndRenderPassCommand final : public OpenGLCommand {
		public:
			explicit OpenGLEndRenderPassCommand(const OpenGLFramebuffer* framebuffer, uint32 subpassIndex) :
				framebuffer(framebuffer),
				subpassIndex(subpassIndex)
			{}

			void execute() const final override {
				const ref<OpenGLRenderPass>& renderPass = std::static_pointer_cast<OpenGLRenderPass>(framebuffer->getRenderPass());
				const uint32 presentSourceAttachmentIndex = renderPass->getPresentSourceAttachmentIndex();

				if (presentSourceAttachmentIndex != max<uint32>()) {
					const GLenum readBuffer = GL_COLOR_ATTACHMENT0 + presentSourceAttachmentIndex;
					const Extent2D extent = framebuffer->getExtent();

					#ifdef OPENGL_USE_DSA
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
					#else
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
					#endif
				}
			}

		private:
			const OpenGLFramebuffer* framebuffer;
			uint32 subpassIndex;
	};
}