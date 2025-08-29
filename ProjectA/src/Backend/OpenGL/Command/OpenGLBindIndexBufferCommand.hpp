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
				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glVertexArrayElementBuffer(vertexArray, indexBuffer);
				#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
					glBindVertexArray(vertexArray);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			const GLuint vertexArray;
			const GLuint indexBuffer;
	};
}