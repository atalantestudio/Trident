#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLCopyBufferToImageCommand.hpp"

namespace ProjectA {
	OpenGLCopyBufferToImageCommand::OpenGLCopyBufferToImageCommand(GLuint sourceBuffer, GLuint destinationImage, GLuint width, GLuint height, GLuint depth, GLenum format) :
		sourceBuffer(sourceBuffer),
		destinationImage(destinationImage),
		width(width),
		height(height),
		depth(depth),
		format(format)
	{}

	void OpenGLCopyBufferToImageCommand::execute() const {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, sourceBuffer);

		// TODO: Use correct type.
		glTextureSubImage2D(destinationImage, depth, 0, 0, width, height, format, GL_UNSIGNED_BYTE, static_cast<const void*>(0));

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
}