#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"

#include "Engine/API/CommandEncoder.hpp"

namespace ProjectA {
	class OpenGLBeginRenderPassCommand : public OpenGLCommand {
		public:
			OpenGLBeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor) :
				descriptor(descriptor)
			{}

			void execute() const override {
				const OpenGLRenderPass& renderPass = *std::static_pointer_cast<OpenGLRenderPass>(descriptor.renderPass);

				glClearColor(descriptor.clearColor[0], descriptor.clearColor[1], descriptor.clearColor[2], descriptor.clearColor[3]);

				if (renderPass.loadOperation == LoadOperation::CLEAR || renderPass.loadOperation == LoadOperation::UNDEFINED) {
					glClear(GL_COLOR_BUFFER_BIT);
				}

				if (renderPass.multisampleFramebuffer != 0) {
					glBindFramebuffer(GL_FRAMEBUFFER, renderPass.multisampleFramebuffer);
					glClear(GL_COLOR_BUFFER_BIT);
				}
			}

		private:
			const BeginRenderPassDescriptor& descriptor;
	};
}