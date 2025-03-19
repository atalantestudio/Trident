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
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, sourceBuffer);

		glTextureSubImage2D(destinationTexture, depth, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<const void*>(0));

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
}