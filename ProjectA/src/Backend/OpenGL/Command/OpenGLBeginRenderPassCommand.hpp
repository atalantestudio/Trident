#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
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

				#if OPENGL_VERSION >= OPENGL_VERSION_3_0
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glFramebuffer->object);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif

				for (uint32 clearValueIndex = 0; clearValueIndex < descriptor.clearValues.size(); clearValueIndex += 1) {
					const ClearValue& clearValue = descriptor.clearValues[clearValueIndex];

					if (clearValueIndex == glFramebuffer->getDepthStencilAttachmentIndex()) {
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glClearNamedFramebufferfi(glFramebuffer->object, GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil); // v4.5
						#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil); // v3.0
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
					else {
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glClearNamedFramebufferfv(glFramebuffer->object, GL_COLOR, clearValueIndex, clearValue.color); // v4.5
						#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
							glClearBufferfv(GL_COLOR, clearValueIndex, clearValue.color); // v3.0
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif
					}
				}

				#if OPENGL_VERSION < OPENGL_VERSION_4_5
					sequence<GLenum> colorAttachments(glFramebuffer->getColorAttachmentCount());

					for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < colorAttachments.count; colorAttachmentIndex += 1) {
						colorAttachments[colorAttachmentIndex] = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
					}

					glDrawBuffers(static_cast<GLsizei>(colorAttachments.count), colorAttachments);
				#endif
			}

		private:
			BeginRenderPassDescriptor descriptor;
	};
}