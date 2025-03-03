#include "pch.hpp"

#include <glad/glad.h>

#include "Platform/OpenGL/Command/OpenGLBindVertexBufferCommand.hpp"

namespace ProjectA {
	OpenGLBindVertexBufferCommand::OpenGLBindVertexBufferCommand(const OpenGLBuffer* vertexBuffer, const GLsizei stride) :
		vertexBuffer(vertexBuffer),
		stride(stride)
	{}

	void OpenGLBindVertexBufferCommand::execute() const {
		glBindVertexBuffer(0, vertexBuffer->object, 0, stride);
	}
}