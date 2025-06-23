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

				glBindFramebuffer(GL_FRAMEBUFFER, glFramebuffer.object);

				// TODO: Restore clear values.
				/* for (uint32 clearValueIndex = 0; clearValueIndex < descriptor.clearValues.size(); clearValueIndex += 1) {
					const ClearValue& clearValue = descriptor.clearValues[clearValueIndex];

					glClearBufferfv(GL_COLOR, clearValueIndex, clearValue.color);
				}

				if (glFramebuffer.hasDepthStencilAttachment()) {
					DEBUG_ASSERT(descriptor.clearValues.size() == glFramebuffer.getColorAttachmentCount() + 1);

					const ClearValue& depthStencilClearValue = descriptor.clearValues.back();

					glClearBufferfi(GL_DEPTH_STENCIL, 0, depthStencilClearValue.depth, depthStencilClearValue.stencil);
				} */

				#ifndef OPENGL_USE_DSA
					sequence<GLenum> colorAttachments(glFramebuffer.getColorAttachmentCount());

					for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < colorAttachments.count; colorAttachmentIndex += 1) {
						colorAttachments[colorAttachmentIndex] = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
					}

					glDrawBuffers(static_cast<GLsizei>(colorAttachments.count), drawBuffers);
				#endif
			}

		private:
			BeginRenderPassDescriptor descriptor;
	};
}