#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLCopyBufferToBufferCommand : public OpenGLCommand {
		public:
			OpenGLCopyBufferToBufferCommand(GLuint sourceBuffer, GLuint destinationBuffer, GLintptr sourceOffset, GLintptr destinationOffset, GLsizeiptr size);

			void execute() const override;

		private:
			const GLuint sourceBuffer;
			const GLuint destinationBuffer;
			const GLintptr sourceOffset;
			const GLintptr destinationOffset;
			const GLsizeiptr size;
	};
}