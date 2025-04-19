#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBeginRenderPassCommand.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"

namespace ProjectA {
	OpenGLBeginRenderPassCommand::OpenGLBeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor) :
		descriptor(descriptor)
	{}

	void OpenGLBeginRenderPassCommand::execute() const {
		const OpenGLRenderPass& renderPass = *std::static_pointer_cast<OpenGLRenderPass>(descriptor.renderPass);

		glClearColor(descriptor.clearColor[0], descriptor.clearColor[1], descriptor.clearColor[2], descriptor.clearColor[3]);

		if (renderPass.loadOperation == LoadOperation::CLEAR || renderPass.loadOperation == LoadOperation::UNDEFINED) {
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}
}