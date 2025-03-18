#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLCopyBufferCommand.hpp"

namespace ProjectA {
	OpenGLCopyBufferCommand::OpenGLCopyBufferCommand(GLuint sourceBuffer, GLuint destinationBuffer, GLintptr sourceOffset, GLintptr destinationOffset, GLsizeiptr size) :
		sourceBuffer(sourceBuffer),
		destinationBuffer(destinationBuffer),
		sourceOffset(sourceOffset),
		destinationOffset(destinationOffset),
		size(size)
	{}

	void OpenGLCopyBufferCommand::execute() const {
		glBindBuffer(GL_COPY_READ_BUFFER, sourceBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, destinationBuffer);

		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, sourceOffset, destinationOffset, size);

		glBindBuffer(GL_COPY_READ_BUFFER, 0);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}
}