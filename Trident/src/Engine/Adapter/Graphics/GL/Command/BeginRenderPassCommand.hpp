// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/NextSubpassCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/API/Graphics/Descriptor/BeginRenderPassDescriptor.hpp"

namespace Trident {
	class GL::BeginRenderPassCommand final : public GL::Command {
		public:
			explicit BeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor) :
				descriptor(descriptor)
			{}

			void execute() const final override {
				const ref<GL::Framebuffer> framebuffer = std::static_pointer_cast<GL::Framebuffer>(descriptor.framebuffer);
				const ref<Trident::RenderPass> renderPass = framebuffer->getRenderPass();

				#if OPENGL_VERSION >= OPENGL_VERSION_3_0
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->framebuffer);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif

				sequence<GLenum> bufs(renderPass->getAttachmentCount());
				uint32 bufIndex = 0;

				while (bufIndex < bufs.count()) {
					bufs[bufIndex] = GL_COLOR_ATTACHMENT0 + bufIndex;

					bufIndex += 1;
				}

				glDrawBuffers(static_cast<GLsizei>(bufs.count()), &bufs[0]);

				uint32 attachmentIndex = 0;

				while (attachmentIndex < descriptor.clearValues.count()) {
					const Attachment& attachment = renderPass->getAttachment(attachmentIndex);

					if (attachment.loadOperation != LoadOperation::CLEAR) {
						attachmentIndex += 1;

						continue;
					}

					const ClearValue& clearValue = descriptor.clearValues[attachmentIndex];

					if (isDepthFormat(attachment.format)) {
						#if OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else if (isSignedIntegerColorFormat(attachment.format)) {
						#if OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferiv(GL_COLOR, attachmentIndex, clearValue.colorSigned);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else if (isUnsignedIntegerColorFormat(attachment.format)) {
						#if OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferuiv(GL_COLOR, attachmentIndex, clearValue.colorUnsigned);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else {
						#if OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferfv(GL_COLOR, attachmentIndex, clearValue.color);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}

					attachmentIndex += 1;
				}

				GL::NextSubpassCommand::setDrawBuffers(framebuffer, 0);
			}

		private:
			BeginRenderPassDescriptor descriptor;
	};
}