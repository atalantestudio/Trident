// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/GL/RenderPass.hpp"

namespace Trident {
	class GL::EndRenderPassCommand final : public GL::Command {
		public:
			explicit EndRenderPassCommand(ref<GL::Framebuffer> framebuffer) :
				framebuffer(framebuffer)
			{}

			void execute() const final override {
				const ref<GL::RenderPass> renderPass = std::static_pointer_cast<GL::RenderPass>(framebuffer->getRenderPass());

				if (renderPass->presentSourceAttachmentIndex == max<uint32>()) {
					if (renderPass->multisample) {
						const Extent2D extent = framebuffer->getExtent();

						// TODO: Should we resolve all color attachments?
						glNamedFramebufferReadBuffer(framebuffer->framebuffer, GL_COLOR_ATTACHMENT0);
						glNamedFramebufferDrawBuffer(framebuffer->resolveFramebuffer, GL_COLOR_ATTACHMENT0);

						glBlitNamedFramebuffer(
							framebuffer->framebuffer,
							framebuffer->resolveFramebuffer,
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

					return;
				}

				const GLenum readBuffer = GL_COLOR_ATTACHMENT0 + renderPass->presentSourceAttachmentIndex;
				const Extent2D extent = framebuffer->getExtent();

				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glNamedFramebufferReadBuffer(framebuffer->framebuffer, readBuffer);

					// TODO(graphics/gl/bind-default-framebuffer)
					// On some devices, glNamedFramebufferDrawBuffer generates a GL_INVALID_OPERATION
					// if the default framebuffer is not bound to GL_DRAW_FRAMEBUFFER.
					// See https://github.com/atalantestudio/Trident/issues/67.
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
					glDrawBuffer(GL_FRONT_LEFT);

					glBlitNamedFramebuffer(
						framebuffer->framebuffer,
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
					glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->framebuffer);
					glReadBuffer(readBuffer);

					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
					glDrawBuffer(GL_FRONT_LEFT);

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
			ref<GL::Framebuffer> framebuffer;
	};
}