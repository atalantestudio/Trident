#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBindVertexBufferCommand.hpp"

namespace ProjectA {
	OpenGLBindVertexBufferCommand::OpenGLBindVertexBufferCommand(GLuint bindingindex, GLuint buffer, GLsizei stride) :
		bindingindex(bindingindex),
		buffer(buffer),
		stride(stride)
	{}

	void OpenGLBindVertexBufferCommand::execute() const {
		// TODO: Use offset.
		glBindVertexBuffer(bindingindex, buffer, 0, stride);
	}
}