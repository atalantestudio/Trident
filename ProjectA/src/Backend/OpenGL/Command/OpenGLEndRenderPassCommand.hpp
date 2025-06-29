#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLEndRenderPassCommand final : public OpenGLCommand {
		public:
			explicit OpenGLEndRenderPassCommand(const OpenGLFramebuffer* framebuffer, uint32 subpassIndex) :
				framebuffer(framebuffer),
				subpassIndex(subpassIndex)
			{}

			void execute() const final override {
				const ref<Subpass>& subpass = framebuffer->getRenderPass()->getSubpass(subpassIndex);

				if (subpass->getPresentAttachmentIndex() != std::numeric_limits<uint32>::max()) {
					glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->object);
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

					// TODO: Why resolveAttachmentCount isn't needed as offset here?
					// TODO: Include subpass offset.
					glReadBuffer(GL_COLOR_ATTACHMENT0 + subpass->getPresentAttachmentIndex());

					glDrawBuffer(GL_FRONT);

					const Extent2D extent = framebuffer->getExtent();

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
				}
			}

		private:
			const OpenGLFramebuffer* framebuffer;
			uint32 subpassIndex;
	};
}