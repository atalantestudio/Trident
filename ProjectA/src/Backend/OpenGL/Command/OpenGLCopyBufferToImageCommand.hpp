#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLCopyBufferToImageCommand : public OpenGLCommand {
		public:
			OpenGLCopyBufferToImageCommand(GLuint sourceBuffer, GLuint destinationImage, GLuint width, GLuint height, GLuint depth, GLenum format);

			void execute() const override;

		private:
			const GLuint sourceBuffer;
			const GLuint destinationImage;
			const GLuint width;
			const GLuint height;
			const GLuint depth;
			const GLenum format;
	};
}