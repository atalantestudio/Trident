#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLFramebuffer.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"

#include "Engine/API/Rendering/CommandEncoder.hpp"

namespace ProjectA {
	class OpenGLBeginRenderPassCommand final : public OpenGLCommand {
		public:
			explicit OpenGLBeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor) :
				descriptor(descriptor)
			{}

			void execute() const final override {
				const OpenGLFramebuffer& glFramebuffer = *std::static_pointer_cast<OpenGLFramebuffer>(descriptor.framebuffer);

				// TODO: Check why the framebuffer still needs to be bound to GL_DRAW_FRAMEBUFFER, even with DSA.
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glFramebuffer.object);

				for (uint32 clearValueIndex = 0; clearValueIndex < descriptor.clearValues.size(); clearValueIndex += 1) {
					const ClearValue& clearValue = descriptor.clearValues[clearValueIndex];

					if (clearValueIndex == glFramebuffer.getDepthStencilAttachmentIndex()) {
						#ifdef OPENGL_USE_DSA
							glClearNamedFramebufferfi(glFramebuffer.object, GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil);
						#else
							glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil);
						#endif
					}
					else {
						#ifdef OPENGL_USE_DSA
							glClearNamedFramebufferfv(glFramebuffer.object, GL_COLOR, clearValueIndex, clearValue.color);
						#else
							glClearBufferfv(GL_COLOR, clearValueIndex, clearValue.color);
						#endif
					}
				}

				#ifndef OPENGL_USE_DSA
					sequence<GLenum> colorAttachments(glFramebuffer.getColorAttachmentCount());

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