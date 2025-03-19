#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLCopyBufferToTextureCommand : public OpenGLCommand {
		public:
			OpenGLCopyBufferToTextureCommand(GLuint sourceBuffer, GLuint destinationTexture, GLuint width, GLuint height, GLuint depth);

			void execute() const override;

		private:
			const GLuint sourceBuffer;
			const GLuint destinationTexture;
			const GLuint width;
			const GLuint height;
			const GLuint depth;
	};
}