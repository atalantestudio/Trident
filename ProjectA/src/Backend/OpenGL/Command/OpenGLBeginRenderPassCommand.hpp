#pragma once

#include "Backend/OpenGL/Command/OpenGLNextSubpassCommand.hpp"
#include "Backend/OpenGL/OpenGLFramebuffer.hpp"

#include "Engine/API/Rendering/Descriptor/BeginRenderPassDescriptor.hpp"

namespace ProjectA {
	class OpenGLBeginRenderPassCommand final : public OpenGLCommand {
		public:
			explicit OpenGLBeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor) :
				descriptor(descriptor)
			{}

			void execute() const final override {
				const ref<OpenGLFramebuffer> glFramebuffer = std::static_pointer_cast<OpenGLFramebuffer>(descriptor.framebuffer);
				const ref<RenderPass>& renderPass = glFramebuffer->getRenderPass();

				#if OPENGL_VERSION >= OPENGL_VERSION_3_0
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glFramebuffer->object);
					glBindFramebuffer(GL_READ_FRAMEBUFFER, glFramebuffer->object);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif

				// Bind every attachment for clearing.
				sequence<GLenum> bufs(renderPass->getAttachmentCount());

				for (uint32 bufIndex = 0; bufIndex < bufs.count(); bufIndex += 1) {
					bufs[bufIndex] = GL_COLOR_ATTACHMENT0 + bufIndex;
				}

				// TODO(draw-buffers): Use glNamedFramebufferDrawBuffers in OpenGL 4.5+.
				glDrawBuffers(static_cast<GLsizei>(bufs.count()), &bufs[0]);

				// Clear attachments with a load operation of LoadOperation::CLEAR.
				for (uint32 attachmentIndex = 0; attachmentIndex < descriptor.clearValues.count(); attachmentIndex += 1) {
					const ClearValue& clearValue = descriptor.clearValues[attachmentIndex];
					const Attachment& attachment = renderPass->getAttachment(attachmentIndex);

					if (attachment.loadOperation != LoadOperation::CLEAR) {
						continue;
					}

					if (isDepthFormat(attachment.format)) {
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glClearNamedFramebufferfi(glFramebuffer->object, GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil);
						#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else if (isSignedIntegerColorFormat(attachment.format)) {
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glClearNamedFramebufferiv(glFramebuffer->object, GL_COLOR, attachmentIndex, pointer_cast<const GLint*>(clearValue.color));
						#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferiv(GL_COLOR, attachmentIndex, clearValue.color);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else if (isUnsignedIntegerColorFormat(attachment.format)) {
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glClearNamedFramebufferuiv(glFramebuffer->object, GL_COLOR, attachmentIndex, pointer_cast<const GLuint*>(clearValue.color));
						#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferuiv(GL_COLOR, attachmentIndex, clearValue.color);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else {
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glClearNamedFramebufferfv(glFramebuffer->object, GL_COLOR, attachmentIndex, clearValue.color);
						#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferfv(GL_COLOR, attachmentIndex, clearValue.color);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
				}

				OpenGLNextSubpassCommand::setDrawBuffers(renderPass->getSubpass(0));
			}

		private:
			const BeginRenderPassDescriptor descriptor;
	};
}