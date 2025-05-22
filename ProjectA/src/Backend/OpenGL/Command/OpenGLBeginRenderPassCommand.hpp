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

				// TODO: Clear attachments.
				DEBUG_ASSERT(descriptor.clearValues.size() == renderPass.clearValueCount);

				/*for (uint32 imageIndex = 0; imageIndex < swapchain.framebuffers.size(); imageIndex += 1) {
					const Attachment& colorAttachment = renderPass.colorResolveAttachments[imageIndex];

					glBindFramebuffer(GL_FRAMEBUFFER, swapchain.framebuffers[imageIndex]);
					glDrawBuffer(GL_COLOR_ATTACHMENT0);

					if (colorAttachment.loadOperation == LoadOperation::CLEAR) {
						const vec4<float32>& color = descriptor.clearValues[imageIndex];

						glClearColor(color[0], color[1], color[2], color[3]);
						glClear(GL_COLOR_BUFFER_BIT);
					}
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
			}

		private:
			const BeginRenderPassDescriptor descriptor;
	};
}