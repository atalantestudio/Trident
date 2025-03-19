#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLCopyBufferToTextureCommand.hpp"

namespace ProjectA {
	OpenGLCopyBufferToTextureCommand::OpenGLCopyBufferToTextureCommand(GLuint sourceBuffer, GLuint destinationTexture, GLuint width, GLuint height, GLuint depth) :
		sourceBuffer(sourceBuffer),
		destinationTexture(destinationTexture),
		width(width),
		height(height),
		depth(depth)
	{}

	void OpenGLCopyBufferToTextureCommand::execute() const {
		//glBindBuffer(GL_COPY_READ_BUFFER, sourceBuffer);
		//glBindBuffer(GL_COPY_WRITE_BUFFER, destinationTexture);

		//glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, sourceOffset, destinationOffset, size);

		//glBindBuffer(GL_COPY_READ_BUFFER, 0);
		//glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}
}