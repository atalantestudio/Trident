#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBindIndexBufferCommand.hpp"

namespace ProjectA {
	OpenGLBindIndexBufferCommand::OpenGLBindIndexBufferCommand(GLuint indexBuffer) :
		indexBuffer(indexBuffer)
	{}

	void OpenGLBindIndexBufferCommand::execute() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	}
}