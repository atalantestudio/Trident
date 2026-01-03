#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/Buffer.hpp"

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
				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glVertexArrayVertexBuffer(vertexArray, bindingIndex, buffer, offset, stride);
				#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
					glBindVertexArray(vertexArray);
					glBindVertexBuffer(bindingIndex, buffer, offset, stride);
				#else
					ENGINE_ERROR_UNSUPPORTED();
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