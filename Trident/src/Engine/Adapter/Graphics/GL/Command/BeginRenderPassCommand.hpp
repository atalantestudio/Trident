// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/NextSubpassCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/API/Rendering/Descriptor/BeginRenderPassDescriptor.hpp"

namespace Trident {
	class GL::BeginRenderPassCommand final : public GL::Command {
		public:
			explicit BeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor) :
				descriptor(descriptor)
			{}

			void execute() const final override {
				const ref<GL::Framebuffer> glFramebuffer = std::static_pointer_cast<GL::Framebuffer>(descriptor.framebuffer);
				const ref<Trident::RenderPass> renderPass = glFramebuffer->getRenderPass();

				#if OPENGL_VERSION >= OPENGL_VERSION_3_0
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glFramebuffer->framebuffer);
					glBindFramebuffer(GL_READ_FRAMEBUFFER, glFramebuffer->framebuffer);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif

				sequence<GLenum> bufs(renderPass->getAttachmentCount());
				uint32 bufIndex = 0;

				while (bufIndex < bufs.count()) {
					bufs[bufIndex] = GL_COLOR_ATTACHMENT0 + bufIndex;

					bufIndex += 1;
				}

				// TODO(draw-buffers): Use glNamedFramebufferDrawBuffers in OpenGL 4.5+.
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
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glClearNamedFramebufferfi(glFramebuffer->framebuffer, GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil);
						#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else if (isSignedIntegerColorFormat(attachment.format)) {
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glClearNamedFramebufferiv(glFramebuffer->framebuffer, GL_COLOR, attachmentIndex, pointer_cast<const GLint*>(clearValue.color));
						#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferiv(GL_COLOR, attachmentIndex, clearValue.color);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else if (isUnsignedIntegerColorFormat(attachment.format)) {
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glClearNamedFramebufferuiv(glFramebuffer->framebuffer, GL_COLOR, attachmentIndex, pointer_cast<const GLuint*>(clearValue.color));
						#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferuiv(GL_COLOR, attachmentIndex, clearValue.color);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else {
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glClearNamedFramebufferfv(glFramebuffer->framebuffer, GL_COLOR, attachmentIndex, clearValue.color);
						#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferfv(GL_COLOR, attachmentIndex, clearValue.color);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}

					attachmentIndex += 1;
				}

				GL::NextSubpassCommand::setDrawBuffers(renderPass->getSubpass(0));
			}

		private:
			BeginRenderPassDescriptor descriptor;
	};
}