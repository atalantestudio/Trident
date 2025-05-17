#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLCopyBufferToImageCommand : public OpenGLCommand {
		public:
			OpenGLCopyBufferToImageCommand(GLuint sourceBuffer, GLuint destinationImage, GLuint width, GLuint height, GLuint depth, GLenum format) :
				sourceBuffer(sourceBuffer),
				destinationImage(destinationImage),
				width(width),
				height(height),
				depth(depth),
				format(format)
			{}

			void execute() const override {
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, sourceBuffer);

				// TODO: Use correct type depending on the texture.
				glTextureSubImage2D(destinationImage, depth, 0, 0, width, height, format, GL_UNSIGNED_BYTE, static_cast<const void*>(0));

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			}

		private:
			const GLuint sourceBuffer;
			const GLuint destinationImage;
			const GLuint width;
			const GLuint height;
			const GLuint depth;
			const GLenum format;
	};
}