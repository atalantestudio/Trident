#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/Command/OpenGLBindVertexBufferCommand.hpp"

namespace ProjectA {
	OpenGLBindVertexBufferCommand::OpenGLBindVertexBufferCommand(const GLuint bindingindex, const GLuint buffer, const GLsizei stride) :
		bindingindex(bindingindex),
		buffer(buffer),
		stride(stride)
	{}

	void OpenGLBindVertexBufferCommand::execute() const {
		// TODO: Use offset.
		glBindVertexBuffer(bindingindex, buffer, 0, stride);
	}
}