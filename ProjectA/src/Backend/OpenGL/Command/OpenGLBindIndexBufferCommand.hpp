#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLBindIndexBufferCommand : public OpenGLCommand {
		public:
			OpenGLBindIndexBufferCommand(GLuint indexBuffer) :
				indexBuffer(indexBuffer)
			{}

			void execute() const override {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			}

		private:
			const GLuint indexBuffer;
	};
}