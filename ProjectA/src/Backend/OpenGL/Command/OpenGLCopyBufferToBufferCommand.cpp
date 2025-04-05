#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLCopyBufferToBufferCommand.hpp"

namespace ProjectA {
	OpenGLCopyBufferToBufferCommand::OpenGLCopyBufferToBufferCommand(GLuint sourceBuffer, GLuint destinationBuffer, GLintptr sourceOffset, GLintptr destinationOffset, GLsizeiptr size) :
		sourceBuffer(sourceBuffer),
		destinationBuffer(destinationBuffer),
		sourceOffset(sourceOffset),
		destinationOffset(destinationOffset),
		size(size)
	{}

	void OpenGLCopyBufferToBufferCommand::execute() const {
		glCopyNamedBufferSubData(sourceBuffer, destinationBuffer, sourceOffset, destinationOffset, size);
	}
}