#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"

#include "Engine/API/Rendering/CommandEncoder.hpp"

namespace ProjectA {
	class OpenGLBeginRenderPassCommand : public OpenGLCommand {
		public:
			OpenGLBeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor) :
				descriptor(descriptor)
			{}

			void execute() const override {
				const OpenGLRenderPass& renderPass = *std::static_pointer_cast<OpenGLRenderPass>(descriptor.renderPass).get();
				const OpenGLSwapchain& swapchain = *std::static_pointer_cast<OpenGLSwapchain>(descriptor.swapchain).get();

				DEBUG_ASSERT(descriptor.clearValues.size() == renderPass.clearValueCount);

				if (renderPass.present && renderPass.getColorAttachmentCount() == 1 && !renderPass.resolve && !renderPass.depthStencil) {
					const ClearValue& clearValue = descriptor.clearValues[0];

					glClearBufferfv(GL_COLOR, 0, clearValue.color);
					glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil);

					return;
				}

				for (uint32 imageIndex = 0; imageIndex < swapchain.imageCount; imageIndex += 1) {
					glBindFramebuffer(GL_FRAMEBUFFER, swapchain.framebuffers[imageIndex]);

					for (uint32 clearValueIndex = 0; clearValueIndex < descriptor.clearValues.size(); clearValueIndex += 1) {
						const ClearValue& clearValue = descriptor.clearValues[clearValueIndex];

						glClearBufferfv(GL_COLOR, clearValueIndex, clearValue.color);
					}

					if (renderPass.depthStencil) {
						const ClearValue& depthStencilClearValue = descriptor.clearValues.back();

						glClearBufferfi(GL_DEPTH_STENCIL, 0, depthStencilClearValue.depth, depthStencilClearValue.stencil);
					}

					fchain<GLenum> drawBuffers(renderPass.getColorAttachmentCount());

					for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < drawBuffers.count; colorAttachmentIndex += 1) {
						drawBuffers[colorAttachmentIndex] = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
					}

					glDrawBuffers(renderPass.getColorAttachmentCount(), &drawBuffers[0]);
				}
			}

		private:
			const BeginRenderPassDescriptor descriptor;
	};
}