#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLBindVertexBufferCommand : public OpenGLCommand {
		public:
			OpenGLBindVertexBufferCommand(GLuint bindingindex, GLuint buffer, GLsizei stride);

			void execute() const override;

		private:
			const GLuint bindingindex;
			const GLuint buffer;
			const GLsizei stride;
	};
}