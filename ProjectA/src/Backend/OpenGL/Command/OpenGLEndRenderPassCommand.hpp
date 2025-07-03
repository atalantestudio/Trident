#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLGraphicsAdapter.hpp"

namespace ProjectA {
	class OpenGLEndRenderPassCommand final : public OpenGLCommand {
		public:
			explicit OpenGLEndRenderPassCommand(const OpenGLFramebuffer* framebuffer, uint32 subpassIndex) :
				framebuffer(framebuffer),
				subpassIndex(subpassIndex)
			{}

			void execute() const final override {
				const ref<Subpass>& subpass = framebuffer->getRenderPass()->getSubpass(subpassIndex);

				if (subpass->getPresentAttachmentIndex() != max<uint32>()) {
					// TODO: Why resolveAttachmentCount isn't needed as offset here?
					// TODO: Include the offset of the subpass.
					const GLenum readBuffer = GL_COLOR_ATTACHMENT0 + subpass->getPresentAttachmentIndex();

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