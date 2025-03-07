#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/Command/OpenGLBeginRenderPassCommand.hpp"

namespace ProjectA {
	OpenGLBeginRenderPassCommand::OpenGLBeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor) :
		descriptor(descriptor)
	{}

	void OpenGLBeginRenderPassCommand::execute() const {
		glClearColor(descriptor.clearColor[0], descriptor.clearColor[1], descriptor.clearColor[2], descriptor.clearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}