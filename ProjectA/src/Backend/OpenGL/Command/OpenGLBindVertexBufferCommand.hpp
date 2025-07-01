#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLBindVertexBufferCommand final : public OpenGLCommand {
		public:
			explicit OpenGLBindVertexBufferCommand(GLuint vertexArray, GLuint bindingIndex, GLuint buffer, GLintptr offset, GLsizei stride) :
				vertexArray(vertexArray),
				bindingIndex(bindingIndex),
				buffer(buffer),
				offset(offset),
				stride(stride)
			{}

			void execute() const final override {
				#ifdef OPENGL_USE_DSA
					glVertexArrayVertexBuffer(vertexArray, bindingIndex, buffer, offset, stride);
				#else
					glBindVertexArray(vertexArray);
					glBindVertexBuffer(bindingIndex, buffer, offset, stride);
				#endif
			}

		private:
			const GLuint vertexArray;
			const GLuint bindingIndex;
			const GLuint buffer;
			const GLintptr offset;
			const GLsizei stride;
	};
}