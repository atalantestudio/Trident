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
				const Extent2D extent = framebuffer->getExtent();

				#if OPENGL_VERSION >= OPENGL_VERSION_3_0
					glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->framebuffer);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif

				if (renderPass->presentSourceAttachmentIndex == max<uint32>()) {
					if (renderPass->multisample) {
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->resolveFramebuffer);

						#if OPENGL_VERSION >= OPENGL_VERSION_3_0
							// TODO(graphics/gl/command-encoder/end-render-pass): Resolve all color attachments, not just the presented one.
							glReadBuffer(GL_COLOR_ATTACHMENT0);
							glDrawBuffer(GL_FRONT_LEFT);
							glBlitFramebuffer(0, 0, extent.width, extent.height, 0, 0, extent.width, extent.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}

					return;
				}

				const GLenum readBuffer = GL_COLOR_ATTACHMENT0 + renderPass->presentSourceAttachmentIndex;

				#if OPENGL_VERSION >= OPENGL_VERSION_3_0
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
					glReadBuffer(readBuffer);
					glDrawBuffer(GL_FRONT_LEFT);
					glBlitFramebuffer(0, 0, extent.width, extent.height, 0, 0, extent.width, extent.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			ref<GL::Framebuffer> framebuffer;
	};
}