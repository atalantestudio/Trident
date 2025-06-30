#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLBindIndexBufferCommand final : public OpenGLCommand {
		public:
			explicit OpenGLBindIndexBufferCommand(GLuint vertexArray, GLuint indexBuffer) :
				vertexArray(vertexArray),
				indexBuffer(indexBuffer)
			{}

			void execute() const final override {
				#ifdef OPENGL_USE_DSA
					glVertexArrayElementBuffer(vertexArray, indexBuffer);
				#else
					glBindVertexArray(vertexArray);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
				#endif
			}

		private:
			const GLuint vertexArray;
			const GLuint indexBuffer;
	};
}