#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBindVertexBufferCommand.hpp"

namespace ProjectA {
	OpenGLBindVertexBufferCommand::OpenGLBindVertexBufferCommand(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) :
		bindingindex(bindingindex),
		buffer(buffer),
		offset(offset),
		stride(stride)
	{}

	void OpenGLBindVertexBufferCommand::execute() const {
		glBindVertexBuffer(bindingindex, buffer, offset, stride);
	}
}